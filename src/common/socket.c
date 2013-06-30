#include "socket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

Socket * Socket_init(int socketfd)
{
	// Allocate struct
	Socket *self = (Socket *)calloc(1, sizeof(Socket));
	// Assign function pointers
	self->destroy = _socket_destroy;
	self->create = _socket_create;
	self->set_peer_addr = _socket_set_peer_addr;
	self->get_peer_addr = _socket_get_peer_addr;
	self->recvfrom = _socket_recvfrom;
	self->sendto = _socket_sendto;
	self->recvready = _socket_recvready;
	self->bind = _socket_bind;
	self->listen = _socket_listen;
	self->accept = _socket_accept;
	self->set_flag = _socket_set_flag;
	self->unset_flag = _socket_unset_flag;
	self->read = _socket_read;
	self->write = _socket_write;
	self->close = _socket_close;

	if (socketfd > 0)
		self->socket = socketfd;

	return self;
}

void _socket_destroy(Socket * self)
{
	if (self->peer_addr != NULL)
	{
		free(self->peer_addr);
	}
	free(self);
}

void _socket_set_peer_addr(Socket *self, struct sockaddr *sockaddr)
{
	self->peer_addr = sockaddr;
}

struct sockaddr * _socket_get_peer_addr(Socket *self)
{
	return self->peer_addr;
}

int _socket_create(Socket *self, int domain, int type)
{
	if (domain <= 0)
	{
		domain = AF_INET;
	}
	if (type <= 0)
	{
		type = SOCK_DGRAM;
	}
	self->domain = domain;
	self->type = type;
	// Create socket
	self->socket = socket(self->domain, self->type, 0);
	if (self->socket == -1)
	{
		return -1;
	}
	return 0;
}

int _socket_bind(Socket *self, char *address, int port)
{
	int ret;
	// Enable address reuse
	int on = 1;
	setsockopt(self->socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (self->domain == AF_INET)
	{
		struct sockaddr_in *sockaddr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
		sockaddr->sin_family = self->domain;
		sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
		sockaddr->sin_port = htons(port);
		// Bind to the port
		ret = bind(self->socket, (struct sockaddr *) sockaddr, sizeof(struct sockaddr_in));
		free(sockaddr);
		return ret;
	}
	else if (self->domain == AF_INET6)
	{
		// Not implemented
	}
	return -1;
}

int _socket_listen(Socket *self, int max_pending)
{
	return listen(self->socket, max_pending);
}

Socket * _socket_accept(Socket *self)
{
	Socket *sockobj;

	if (self->domain == AF_INET)
	{
		struct sockaddr_in *sockaddr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
		socklen_t len = sizeof(struct sockaddr_in);
		int newsock = accept(self->socket, (struct sockaddr *)sockaddr, &len);
		if (newsock <= 0)
		{
			return NULL;
		}
		// Create new Socket object here with this socket
		sockobj = Socket_init(newsock);
		sockobj->set_peer_addr(sockobj, (struct sockaddr *)sockaddr);
	}
	else if (self->domain == AF_INET6)
	{
		// Not implemented yet
		return NULL;
	}

	// Copy type/domain from listening socket
	sockobj->type = self->type;
	sockobj->domain = self->domain;
	return (void *)sockobj;
}

int _socket_recvfrom(Socket *self, char *buf, int buf_size, struct sockaddr *sockaddr, unsigned int *sockaddr_size)
{
	int n = recvfrom(self->socket, buf, buf_size, 0, sockaddr, sockaddr_size);
	if (n < 0)
	{
		printf("socket_recvfrom(): %s\n", strerror(errno));
		return -1;
	}
	buf[n] = 0;
	return n;
}

int _socket_sendto(Socket *self, char *buf, struct sockaddr *sockaddr, unsigned int sockaddr_size)
{
	int n = sendto(self->socket, buf, strlen(buf), 0, sockaddr, sockaddr_size);
	if (n < 0)
	{
		printf("socket_sendto(): %s\n", strerror(errno));
	}
	return n;
}

int _socket_recvready(Socket *self, int timeout_sec)
{
	fd_set rfds;
	int n;

	struct timeval *timeout = NULL;

	if (timeout_sec >= 0)
	{
		timeout = (struct timeval *)calloc(1, sizeof(struct timeval));
		timeout->tv_sec = timeout_sec;
		timeout->tv_usec = 0;
	}

	FD_ZERO(&rfds);
	FD_SET(self->socket, &rfds);

	n = select(self->socket + 1, &rfds, NULL, NULL, timeout);

	if (timeout != NULL)
	{
		free(timeout);
	}
	if (n < 0)
	{
		printf("socket_recvready(): %s\n", strerror(errno));
		return -1;
	}
	if (n == 0)
	{
		// Timeout
	}
	if (FD_ISSET(self->socket, &rfds))
	{
		return 1;
	}
	return 0;
}

void _socket_set_flag(Socket *self, int flag)
{
	int flags = fcntl(self->socket, F_GETFL, 0);
	fcntl(self->socket, F_SETFL, flags | flag);
}

void _socket_unset_flag(Socket *self, int flag)
{
	int flags = fcntl(self->socket, F_GETFL, 0);
	fcntl(self->socket, F_SETFL, flags & ~flag);
}

int _socket_read(Socket *self, char *buf, int buflen)
{
	int n = 0;
	n = read(self->socket, buf, buflen);
	if (n >= 0)
	{
		// Null terminate the string since read() doesn't
		buf[n] = 0;
	}
	return n;
}

int _socket_write(Socket *self, char *buf, int buflen)
{
	return write(self->socket, buf, buflen);
}

int _socket_close(Socket * self)
{
	return close(self->socket);
}
