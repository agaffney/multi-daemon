#include "common/socket.h"
#include "common/dispatcher.h"

int echo_entry(config_opt[]);
int echo_dispatcher_callback(Dispatcher *, Socket *);
int echo_recv_ready_udp(Socket *);
int echo_recv_ready_tcp(Socket *);

