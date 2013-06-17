#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
