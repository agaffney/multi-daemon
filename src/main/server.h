typedef struct {
	char proto[5];
	int port;
	int (*recv_callback)(char *, void *);
	int (*recv_ready_callback)();
} server_info;

typedef struct {
	server_info *server_info;
	struct sockaddr *client_addr;
	int socket_fd;
	char client_ip[16];
	int client_port;
} server_callback_info;

int server_start(server_info *);
int server_tcp_start(server_info *);
int server_udp_start(server_info *);
int server_sendto(char *, server_callback_info *);