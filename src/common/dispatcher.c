#include "dispatcher.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

// This is here so that MAP_ANONYMOUS is usable
#define __USE_MISC
#include <sys/mman.h>

Dispatcher * Dispatcher_init(int worker_model, int num_workers)
{
	Dispatcher * self = (Dispatcher *)calloc(1, sizeof(Dispatcher));
	self->_worker_model = worker_model;
	self->_num_workers = num_workers;
	// Assign function pointers
	self->destroy = _dispatcher_destroy;
	self->add_listener = _dispatcher_add_listener;
	self->run = _dispatcher_run;
	return self;
}

void _dispatcher_destroy(Dispatcher * self)
{
	for (int i = 0; i < self->_listener_count; i++)
	{
		free(self->_listeners[i]);
	}
	free(self);
}

int _dispatcher_add_listener(Dispatcher * self, Socket * sock, dispatcher_callback_func poll_callback, dispatcher_callback_func run_callback, dispatcher_callback_func cleanup_callback, void * cbarg)
{
	if (self->_listener_count >= _DISPATCHER_MAX_LISTENERS)
	{
		return 1;
	}
	dispatcher_listener * listener = (dispatcher_listener *)calloc(1, sizeof(dispatcher_listener));
	listener->sock = sock;
	listener->poll_callback = poll_callback;
	listener->run_callback = run_callback;
	listener->cleanup_callback = cleanup_callback;
	self->_listeners[self->_listener_count] = listener;
	self->_listener_count++;
	return 0;
}

int _dispatcher_run(Dispatcher * self)
{
	// place semaphores in shared memory
	sem_t * poll_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(poll_sem, 1, 1))
	{
		perror("failed to initialize poll semaphore");
		return 1;
	}
	switch (self->_worker_model)
	{
		case DISPATCHER_WORKER_MODEL_SINGLE:
		case DISPATCHER_WORKER_MODEL_POSTFORK:
		{
			dispatcher_worker_info * worker_info = (dispatcher_worker_info *)calloc(1, sizeof(worker_info));
			worker_info->worker_num = -1;
			worker_info->dispatcher = self;
			worker_info->poll_sem = poll_sem;
			_dispatcher_worker_run((void *)worker_info);
			break;
		}
		case DISPATCHER_WORKER_MODEL_PREFORK:
			// Create a mutex/semaphore, fork off workers, then call worker_run()
			for (int i = 0; i < self->_num_workers; i++)
			{
				dispatcher_worker_info * worker_info = (dispatcher_worker_info *)calloc(1, sizeof(worker_info));
				worker_info->worker_num = i;
				worker_info->dispatcher = self;
				worker_info->poll_sem = poll_sem;
				pid_t child_pid = fork();
				if (child_pid < 0)
				{
					perror("worker fork failed");
					return 1;
				}
				else if (child_pid == 0)
				{
					// Child
					_dispatcher_worker_run((void *)worker_info);
				}
				else
				{
					// Parent
				}
			}
			int child_status;
			while (1)
			{
				waitpid(-1, &child_status, 0);
			}
			break;
		case DISPATCHER_WORKER_MODEL_THREAD:
		{
			// Create a mutex/semaphore and start threads with worker_run()
			pthread_t thread_id;
			for (int i = 0; i < self->_num_workers; i++)
			{
				dispatcher_worker_info * worker_info = (dispatcher_worker_info *)calloc(1, sizeof(worker_info));
				worker_info->worker_num = i;
				worker_info->dispatcher = self;
				worker_info->poll_sem = poll_sem;
				if (pthread_create(&thread_id, NULL, _dispatcher_worker_run, (void *)worker_info))
				{
					perror("worker thread creation failed");
					return 1;
				}
			}
			// Lazy way to just wait on threads
			void * res;
			if (pthread_join(thread_id, &res))
			{
				perror("pthread_join() failed");
				return 1;
			}
			break;
		}
		default:
			return 1;
	}
	return 0;
}

int _dispatcher_build_listener_epoll_set(Dispatcher * self)
{
	struct epoll_event ev;
	int epollfd = epoll_create(1);

	for (int i = 0; i < self->_listener_count; i++)
	{
		ev.events = EPOLLIN;
		ev.data.fd = self->_listeners[i]->sock->socket;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, self->_listeners[i]->sock->socket, &ev) == -1) {
			perror("epoll_ctl failed");
			return -1;
		}
	}

	return epollfd;
}

int _dispatcher_poll_listeners(Dispatcher * self, int epollfd, struct epoll_event *events, int max_events)
{
	return epoll_wait(epollfd, events, max_events, -1);
}

void * _dispatcher_worker_run(void * arg)
{
	int child_count = 0;
	int child_status;
	pid_t fork_child_pid, wait_child_pid;
	dispatcher_worker_info * worker_info = arg;
	Dispatcher * self = worker_info->dispatcher;
	// Build epoll set
	int epollfd = _dispatcher_build_listener_epoll_set(self);
	struct epoll_event events[10];

	while (1)
	{
		int ready_fds;
		if (self->_worker_model == DISPATCHER_WORKER_MODEL_POSTFORK)
		{
			// Naively clean up after children
			wait_child_pid = waitpid(-1, &child_status, WNOHANG);
			if (wait_child_pid > 0)
			{
				child_count--;
			}
		}
		// Look for sockets that are ready for action
		// Block on the semaphore
		sem_wait(worker_info->poll_sem);
		ready_fds = _dispatcher_poll_listeners(self, epollfd, events, 10);
		if (ready_fds <= 0)
		{
			// Release the semaphore
			sem_post(worker_info->poll_sem);
			continue;
		}
		// Figure out which listeners are ready
		int found_fd;
		for (int i = 0; i < ready_fds; i++)
		{
			found_fd = 0;
			for (int j = 0; j < self->_listener_count; j++)
			{
				dispatcher_listener * tmp_listener = self->_listeners[j];
				if (tmp_listener->sock->socket == events[i].data.fd)
				{
					dispatcher_callback_info cb_info = { self, tmp_listener->sock, {}, 0, tmp_listener->cbarg };
					if (tmp_listener->poll_callback(&cb_info))
					{
						// Something went wrong in the poll_callback
						sem_post(worker_info->poll_sem);
						break;
					}
					// Release the semaphore
					sem_post(worker_info->poll_sem);
					if (self->_worker_model == DISPATCHER_WORKER_MODEL_POSTFORK)
					{
						// fork it
						fork_child_pid = fork();
						if (fork_child_pid > 0)
						{
							// Parent
							cb_info.extra_flag = 1;
							tmp_listener->cleanup_callback(&cb_info);
							printf("[%d] forked off child with PID %d\n", getpid(), fork_child_pid);
							child_count++;
							break;
						}
						else
						{
							// Child
							worker_info->worker_num = child_count;
						}
					}
//					printf("[%d] Handling request in worker %d\n", getpid(), worker_info->worker_num);
					tmp_listener->run_callback(&cb_info);
					tmp_listener->cleanup_callback(&cb_info);
					if (self->_worker_model == DISPATCHER_WORKER_MODEL_POSTFORK && fork_child_pid == 0)
					{
						// Child
						printf("[%d] Calling _exit() in child\n", getpid());
						_exit(0);
					}
					found_fd = 1;
					break;
				}
			}
			if (found_fd)
			{
				// Go back to the top loop if we handled a request
				break;
			}
		}
		// Release the semaphore if we didn't find anything
		sem_post(worker_info->poll_sem);
	}

	return NULL;
}


