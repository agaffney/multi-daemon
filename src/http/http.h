#include "common/socket.h"
#include "common/dispatcher.h"
#include "common/hash.h"

int http_entry(Hash *);
int http_dispatcher_poll_callback(dispatcher_callback_info *);
int http_dispatcher_run_callback(dispatcher_callback_info *);
int http_dispatcher_cleanup_callback(dispatcher_callback_info *);

