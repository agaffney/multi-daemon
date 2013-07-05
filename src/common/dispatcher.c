#include "dispatcher.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

Dispatcher * Dispatcher_init(int worker_model, int num_workers)
{
	Dispatcher * self = (Dispatcher *)calloc(1, sizeof(Dispatcher));
	self->_worker_model = worker_model;
	self->_num_workers = num_workers;
	// Assign function pointers
	self->destroy = _dispatcher_destroy;
	self->add_listener = _dispatcher_add_listener;
	self->run = _dispatcher_run;
	self->worker_run = _dispatcher_worker_run;
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

int _dispatcher_add_listener(Dispatcher * self, Socket * sock, int (*callback)(Dispatcher *, Socket *))
{
	if (self->_listener_count >= _DISPATCHER_MAX_LISTENERS)
	{
		return 1;
	}
	dispatcher_listener * listener = (dispatcher_listener *)calloc(1, sizeof(dispatcher_listener));
	listener->sock = sock;
	listener->callback = callback;
	self->_listeners[self->_listener_count] = listener;
	self->_listener_count++;
	return 0;
}

int _dispatcher_run(Dispatcher * self)
{
	switch (self->_worker_model)
	{
		case DISPATCHER_WORKER_MODEL_NONE:
			self->worker_run(self, 1);
			break;
		case DISPATCHER_WORKER_MODEL_POSTFORK:
			self->worker_run(self, 1);
			break;
		case DISPATCHER_WORKER_MODEL_PREFORK:
			// Create a mutex/semaphore, fork off workers, then call worker_run()
			break;
		case DISPATCHER_WORKER_MODEL_THREAD:
			// Create a mutex/semaphore and start threads with worker_run()
			break;
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
	timeout->tv_sec = 0;
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

int _dispatcher_worker_run(Dispatcher * self, int worker_num)
{
	switch (self->_worker_model)
	{
		case DISPATCHER_WORKER_MODEL_NONE:
		case DISPATCHER_WORKER_MODEL_POSTFORK:
			while (1)
			{
				int ready_fds, child_status;
				int last_ready_fd = -1;
				// Naively clean up after children
				waitpid(-1, &child_status, WNOHANG);
				// Look for sockets that are ready for action
				fd_set * rfds = (fd_set *)calloc(1, sizeof(fd_set));
				int max_fd = self->build_listener_fdset(self, rfds);
				ready_fds = self->poll_listeners(self, rfds, max_fd);
				free(rfds);
				if (ready_fds <= 0)
				{
					continue;
				}
				printf("_dispatcher_worker_run(): we found a listener that was ready!\n");
				// Figure out which listeners are ready
				for (int i = 0; i < ready_fds; i++)
				{
					for (int j = last_ready_fd + 1; j <= max_fd; j++)
					{
						
						if (FD_ISSET(j, rfds))
						{
							last_ready_fd = j;
							dispatcher_listener * tmp_listener = self->find_listener(self, j);
							Socket * newsock = tmp_listener->sock->accept(tmp_listener->sock);
							if (self->_worker_model == DISPATCHER_WORKER_MODEL_POSTFORK)
							{
								// fork it
								pid_t child_pid = fork();
								if (child_pid > 0)
								{
									// Parent
									newsock->close(newsock);
									newsock->destroy(newsock);
									break;
								}
								else
								{
									// Child
									tmp_listener->sock->close(tmp_listener->sock);
									tmp_listener->sock->destroy(tmp_listener->sock);
								}
							}
							tmp_listener->callback(self, newsock);
							break;
						}
					}

				}
			}
			break;
	}

	return 0;
}
