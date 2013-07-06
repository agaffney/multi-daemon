#include "common/socket.h"
#include "common/dispatcher.h"

int echo_entry(config_opt[]);
int echo_dispatcher_callback(dispatcher_callback_info *);
int echo_recv_ready_udp(Socket *, sem_t *);
int echo_recv_ready_tcp(Socket *);

