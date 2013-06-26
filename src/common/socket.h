#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

struct _Socket {
	int socket;
	int domain;
	int type;
	struct sockaddr *peer_addr;
	int (*init)();
	void (*set_peer_addr)(struct _Socket *, struct sockaddr *);
	struct sockaddr * (*get_peer_addr)(struct _Socket *);
	int (*create)(struct _Socket *, int, int);
	int (*recvfrom)(struct _Socket *, char *, int, struct sockaddr *, unsigned int *);
	int (*sendto)(struct _Socket *, char *, struct sockaddr *, unsigned int);
	int (*recvready)(struct _Socket *, int);
	int (*bind)(struct _Socket *, char *, int);
	int (*listen)(struct _Socket *, int);
	struct _Socket * (*accept)(struct _Socket *);
	void (*set_flag)(struct _Socket *, int);
	void (*unset_flag)(struct _Socket *, int);
	int (*read)(struct _Socket *, char *, int);
	int (*write)(struct _Socket *, char *, int);
	int (*close)(struct _Socket *);
};
typedef struct _Socket Socket;

Socket * socket_init(int);
void socket_set_peer_addr(Socket *, struct sockaddr *);
struct sockaddr * socket_get_peer_addr(Socket *);
int socket_create(Socket *, int, int);
int socket_recvfrom(Socket *, char *, int, struct sockaddr *, unsigned int *);
int socket_sendto(Socket *, char *, struct sockaddr *, unsigned int);
int socket_recvready(Socket *, int);
int socket_bind(Socket *, char *, int);
int socket_listen(Socket *, int);
Socket * socket_accept(Socket *);
void socket_set_flag(Socket *, int);
void socket_unset_flag(Socket *, int);
int socket_read(Socket *, char *, int);
int socket_write(Socket *, char *, int);
int _socket_close(Socket *);

#endif
