#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
	char proto[5];
	int port;
	int (*recv_ready_callback)();
} server_info;

int server_start(server_info *);
int server_tcp_start(server_info *);
int server_udp_start(server_info *);

typedef struct {
	int socket;
	int (*init)();
	int (*recvfrom)();
	int (*sendto)();
	int (*recvready)();
} Socket;

int socket_init(Socket *, int);
int socket_recvfrom(Socket *, char *, int, struct sockaddr *, unsigned int *);
int socket_sendto(Socket *, char *, struct sockaddr *, unsigned int);
int socket_recvready(Socket *, int);
