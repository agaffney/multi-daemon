#include "common/socket.h"
#include "common/dispatcher.h"
#include "common/hash.h"

#define ECHO_MAX_UDP_PACKET_SIZE 600

int echo_entry(Hash *);
int echo_dispatcher_poll_callback(dispatcher_callback_info *);
int echo_dispatcher_run_callback(dispatcher_callback_info *);
int echo_dispatcher_cleanup_callback(dispatcher_callback_info *);

