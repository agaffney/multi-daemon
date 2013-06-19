#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

typedef struct {
	int socket;
	int domain;
	int type;
	struct sockaddr *peer_addr;
	int (*init)();
	void (*set_peer_addr)();
	struct sockaddr * (*get_peer_addr)();
	int (*create)();
	int (*recvfrom)();
	int (*sendto)();
	int (*recvready)();
	int (*bind)();
	int (*listen)();
	void * (*accept)();
	void (*set_flag)();
	void (*unset_flag)();
} Socket;

Socket * socket_init(int);
void socket_set_peer_addr(Socket *, struct sockaddr *);
struct sockaddr * socket_get_peer_addr(Socket *);
int socket_create(Socket *, int, int);
int socket_recvfrom(Socket *, char *, int, struct sockaddr *, unsigned int *);
int socket_sendto(Socket *, char *, struct sockaddr *, unsigned int);
int socket_recvready(Socket *, int);
int socket_bind(Socket *, char *, int);
int socket_listen(Socket *, int);
void * socket_accept(Socket *);
void socket_set_flag(Socket *, int);
void socket_unset_flag(Socket *, int);
