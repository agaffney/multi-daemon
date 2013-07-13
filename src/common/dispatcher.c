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
	self->build_listener_fdset = _dispatcher_build_listener_fdset;
	self->poll_listeners = _dispatcher_poll_listeners;
	self->find_listener = _dispatcher_find_listener;
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

int _dispatcher_add_listener(Dispatcher * self, Socket * sock, dispatcher_callback_func poll_callback, dispatcher_callback_func run_callback, dispatcher_callback_func cleanup_callback)
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
	sem_t * recv_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sem_init(recv_sem, 1, 1))
	{
		perror("failed to initialize recv semaphore");
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
			worker_info->recv_sem = recv_sem;
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
				worker_info->recv_sem = recv_sem;
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

int _dispatcher_build_listener_fdset(Dispatcher * self, fd_set * rfds)
{
	int max_fd = -1;

	FD_ZERO(rfds);
	for (int i = 0; i < self->_listener_count; i++)
	{
		FD_SET(self->_listeners[i]->sock->socket, rfds);
		if (self->_listeners[i]->sock->socket > max_fd)
		{
			max_fd = self->_listeners[i]->sock->socket;
		}
	}

	return max_fd;
}

int _dispatcher_poll_listeners(Dispatcher * self, fd_set * rfds, int max_fd)
{
	struct timeval *timeout = (struct timeval *)calloc(1, sizeof(struct timeval));
	timeout->tv_sec = 5;
	timeout->tv_usec = 0;

	int n = select(max_fd + 1, rfds, NULL, NULL, timeout);

	free(timeout);

	return n;

/*
	if (n < 0)
	{
		printf("_dispatcher_poll_listeners(): %s\n", strerror(errno));
		return -1;
	}
	if (n == 0)
	{
		// Timeout
	}
	if (FD_ISSET(self->socket, &rfds))
	{
		return 1;
	}
*/
}

dispatcher_listener * _dispatcher_find_listener(Dispatcher * self, int socket_fd)
{
	for (int i = 0; i < self->_listener_count; i++)
	{
		if (self->_listeners[i]->sock->socket == socket_fd)
		{
			return self->_listeners[i];
		}
	}
	return NULL;
}

void * _dispatcher_worker_run(void * arg)
{
	int child_count = 0;
	int child_status;
	pid_t fork_child_pid, wait_child_pid;
	dispatcher_worker_info * worker_info = arg;
	fd_set * rfds = (fd_set *)calloc(1, sizeof(fd_set));
	Dispatcher * self = worker_info->dispatcher;

	while (1)
	{
		int ready_fds;
		int last_ready_fd = -1;
		if (self->_worker_model == DISPATCHER_WORKER_MODEL_POSTFORK)
		{
			// Naively clean up after children
			wait_child_pid = waitpid(-1, &child_status, WNOHANG);
			if (wait_child_pid > 0)
			{
				child_count--;
			}
			else if (wait_child_pid == -1)
			{
				perror("waitpid() failed");
			}
		}
		// Look for sockets that are ready for action
		int max_fd = self->build_listener_fdset(self, rfds);
		// Block on the semaphore
		sem_wait(worker_info->poll_sem);
		ready_fds = self->poll_listeners(self, rfds, max_fd);
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
			for (int j = last_ready_fd + 1; j <= max_fd; j++)
			{
				if (FD_ISSET(j, rfds))
				{
					last_ready_fd = j;
					dispatcher_listener * tmp_listener = self->find_listener(self, j);
					dispatcher_callback_info cb_info = { self, tmp_listener->sock, {} };
					if (tmp_listener->poll_callback(&cb_info))
					{
						// Something went wrong in the poll_callback
						sem_post(worker_info->poll_sem);
						printf("[%d] something went wrong in the poll callback\n", getpid());
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
//							tmp_listener->cleanup_callback(&cb_info);
							child_count++;
							break;
						}
						else
						{
							// Child
						}
					}
					printf("[%d] Handling request in worker %d\n", getpid(), worker_info->worker_num);
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
	}

	free(rfds);

	return NULL;
}


