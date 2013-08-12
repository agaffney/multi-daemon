#include "common/socket.h"
#include "common/dispatcher.h"
#include "common/hash.h"

#define HTTP_DOCROOT_SIZE 256

struct _http_global_info {
	char docroot[HTTP_DOCROOT_SIZE];
};

typedef struct _http_global_info http_global_info;

int http_start(Hash *);
int http_dispatcher_poll_callback(dispatcher_callback_info *);
int http_dispatcher_run_callback(dispatcher_callback_info *);
int http_dispatcher_cleanup_callback(dispatcher_callback_info *);

