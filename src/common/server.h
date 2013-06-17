typedef struct {
	char proto[5];
	int port;
	int (*recv_ready_callback)();
} server_info;

int server_start(server_info *);
int server_tcp_start(server_info *);
int server_udp_start(server_info *);

