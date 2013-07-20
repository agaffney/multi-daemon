#ifndef DISPATCHER_H_INCLUDED
#define DISPATCHER_H_INCLUDED

#include "socket.h"

#include <semaphore.h>
#include <sys/epoll.h>

#define _DISPATCHER_MAX_LISTENERS 100

#define DISPATCHER_WORKER_MODEL_SINGLE   0
#define DISPATCHER_WORKER_MODEL_POSTFORK 1
#define DISPATCHER_WORKER_MODEL_PREFORK  2
#define DISPATCHER_WORKER_MODEL_THREAD   3

struct _dispatcher_callback_info {
	struct _Dispatcher * dispatcher;
	Socket * sock;
	void * data[5];
	int extra_flag;
};

typedef struct _dispatcher_callback_info dispatcher_callback_info;

typedef int (*dispatcher_callback_func)(dispatcher_callback_info *);

struct _Dispatcher {
	struct _dispatcher_listener * _listeners[_DISPATCHER_MAX_LISTENERS];
	int _listener_count;
	int _worker_model;
	int _num_workers;
	void (*destroy)(struct _Dispatcher *);
	int (*add_listener)(struct _Dispatcher *, Socket *, dispatcher_callback_func, dispatcher_callback_func, dispatcher_callback_func);
	int (*run)(struct _Dispatcher *);
};

typedef struct _Dispatcher Dispatcher;

struct _dispatcher_listener {
	Socket * sock;
	dispatcher_callback_func poll_callback;
	dispatcher_callback_func run_callback;
	dispatcher_callback_func cleanup_callback;
};

typedef struct _dispatcher_listener dispatcher_listener;

struct _dispatcher_worker_info {
	int worker_num;
	Dispatcher * dispatcher;
	sem_t * poll_sem;
};

typedef struct _dispatcher_worker_info dispatcher_worker_info;

Dispatcher * Dispatcher_init(int, int);
void _dispatcher_destroy(Dispatcher *);
int _dispatcher_add_listener(Dispatcher *, Socket *, dispatcher_callback_func, dispatcher_callback_func, dispatcher_callback_func);
int _dispatcher_run(Dispatcher *);
void * _dispatcher_worker_run(void *);
int _dispatcher_build_listener_epoll_set(Dispatcher *);
int _dispatcher_poll_listeners(Dispatcher *, int, struct epoll_event *, int);

#endif
