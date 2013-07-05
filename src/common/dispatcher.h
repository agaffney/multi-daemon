#include "socket.h"

#include <sys/select.h>
#include <semaphore.h>

#define _DISPATCHER_MAX_LISTENERS 100

#define DISPATCHER_WORKER_MODEL_SINGLE   0
#define DISPATCHER_WORKER_MODEL_POSTFORK 1
#define DISPATCHER_WORKER_MODEL_PREFORK  2
#define DISPATCHER_WORKER_MODEL_THREAD   3

struct _Dispatcher {
	struct _dispatcher_listener * _listeners[_DISPATCHER_MAX_LISTENERS];
	int _listener_count;
	int _worker_model;
	int _num_workers;
	void (*destroy)(struct _Dispatcher *);
	int (*add_listener)(struct _Dispatcher *, Socket *, int (*callback)(struct _Dispatcher *, Socket *));
	int (*run)(struct _Dispatcher *);
	int (*worker_run)(struct _Dispatcher *, int, sem_t *);
	int (*build_listener_fdset)(struct _Dispatcher *, fd_set *);
	int (*poll_listeners)(struct _Dispatcher *, fd_set *, int);
	struct _dispatcher_listener * (*find_listener)(struct _Dispatcher *, int);
};

typedef struct _Dispatcher Dispatcher;

struct _dispatcher_listener {
	Socket * sock;
	int (*callback)(Dispatcher *, Socket *);
};

typedef struct _dispatcher_listener dispatcher_listener;

Dispatcher * Dispatcher_init(int, int);
void _dispatcher_destroy(Dispatcher *);
int _dispatcher_add_listener(Dispatcher *, Socket *, int (*callback)(Dispatcher *, Socket *));
int _dispatcher_run(Dispatcher *);
int _dispatcher_worker_run(Dispatcher *, int, sem_t *);
int _dispatcher_build_listener_fdset(Dispatcher *, fd_set *);
int _dispatcher_poll_listeners(Dispatcher *, fd_set *, int);
dispatcher_listener * _dispatcher_find_listener(Dispatcher *, int);

