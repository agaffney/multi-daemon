#include "dispatcher.h"

#include <stdlib.h>

Dispatcher * Dispatcher_init()
{
	Dispatcher * self = (Dispatcher *)calloc(1, sizeof(Dispatcher));

	// Assign function pointers
	self->destroy = _dispatcher_destroy;
	return self;
}

void _dispatcher_destroy(Dispatcher * self)
{
	free(self);
}
