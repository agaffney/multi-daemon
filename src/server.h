typedef struct {
	char proto[5];
	int port;

} type_server_info;

int server_start(type_server_info);
int server_tcp_start(type_server_info);
int server_udp_start(type_server_info);
