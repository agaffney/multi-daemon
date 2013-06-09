typedef struct _server_info {
	char proto[5];
	int port;
	int (*recv_callback)(char *, void *);
} type_server_info;

typedef struct _server_callback_info {
	type_server_info *server_info;
	struct sockaddr *client_addr;
	int socket_fd;
	char client_ip[16];
	int client_port;
} type_server_callback_info;

int server_start(type_server_info);
int server_tcp_start(type_server_info);
int server_udp_start(type_server_info);
int server_sendto(char *, type_server_callback_info *);
