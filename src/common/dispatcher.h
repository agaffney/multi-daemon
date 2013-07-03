#include "socket.h"

#define _DISPATCHER_MAX_LISTENERS 100

#define DISPATCHER_WORKER_MODEL_NONE     0
#define DISPATCHER_WORKER_MODEL_POSTFORK 1
#define DISPATCHER_WORKER_MODEL_PREFORK  2
#define DISPATCHER_WORKER_MODEL_THREAD   3

struct _Dispatcher {
	Socket * _listeners[_DISPATCHER_MAX_LISTENERS];
	int _listener_count;
	int _worker_model;
	void (*destroy)(struct _Dispatcher *);
	int (*add_listener)(struct _Dispatcher *, Socket *, int (*callback)(struct _Dispatcher *, Socket *));
};

typedef struct _Dispatcher Dispatcher;

Dispatcher * Dispatcher_init(unsigned short int);
void _dispatcher_destroy(Dispatcher *);
int _dispatcher_add_listener(Dispatcher *, Socket *, int (*callback)(Dispatcher *, Socket *));
