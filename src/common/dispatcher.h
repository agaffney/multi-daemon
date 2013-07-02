struct _Dispatcher {

	void (*destroy)(struct _Dispatcher *);
};

typedef struct _Dispatcher Dispatcher;

Dispatcher * Dispatcher_init();
void _dispatcher_destroy(Dispatcher *);
