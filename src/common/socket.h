#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
	int socket;
	int domain;
	int type;
	int (*init)();
	int (*recvfrom)();
	int (*sendto)();
	int (*recvready)();
	int (*bind)();
	int (*listen)();
	int (*accept)();
} Socket;

Socket * socket_init(int, int);
int socket_recvfrom(Socket *, char *, int, struct sockaddr *, unsigned int *);
int socket_sendto(Socket *, char *, struct sockaddr *, unsigned int);
int socket_recvready(Socket *, int);
int socket_bind(Socket *, char *, int);
int socket_listen(Socket *, int);
int socket_accept(Socket *);
