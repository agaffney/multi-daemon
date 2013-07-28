#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SOCKET_READ_BUF_LEN 4096

struct _Socket {
	// socket fd
	int socket;
	// socket attributes
	int domain;
	int type;
	// flag for whether we've called close()
	int _closed;
	// read buffer
	int _read_buf_offset;
	int _read_buf_len;
	char _read_buf[SOCKET_READ_BUF_LEN];
	// client address
	struct sockaddr *peer_addr;
	// function pointers
	void (*destroy)(struct _Socket *);
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

Socket * Socket_init(int);
void _socket_destroy(Socket *);
void _socket_set_peer_addr(Socket *, struct sockaddr *);
struct sockaddr * _socket_get_peer_addr(Socket *);
int _socket_create(Socket *, int, int);
int _socket_recvfrom(Socket *, char *, int, struct sockaddr *, unsigned int *);
int _socket_sendto(Socket *, char *, struct sockaddr *, unsigned int);
int _socket_recvready(Socket *, int);
int _socket_bind(Socket *, char *, int);
int _socket_listen(Socket *, int);
Socket * _socket_accept(Socket *);
void _socket_set_flag(Socket *, int);
void _socket_unset_flag(Socket *, int);
int _socket_read(Socket *, char *, int);
int _socket_write(Socket *, char *, int);
int _socket_close(Socket *);

#endif
