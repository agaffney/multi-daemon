#include "common/socket.h"
#include "common/dispatcher.h"

#define ECHO_MAX_UDP_PACKET_SIZE 600

int echo_entry(config_opt[]);
int echo_dispatcher_poll_callback(dispatcher_callback_info *);
int echo_dispatcher_run_callback(dispatcher_callback_info *);

