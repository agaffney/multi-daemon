#include "dispatcher.h"

#include <stdlib.h>

Dispatcher * Dispatcher_init(unsigned short int worker_model)
{
	Dispatcher * self = (Dispatcher *)calloc(1, sizeof(Dispatcher));
	self->_worker_model = worker_model;
	// Assign function pointers
	self->destroy = _dispatcher_destroy;
	self->add_listener = _dispatcher_add_listener;
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