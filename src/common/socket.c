#include "socket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

Socket * socket_init(int domain, int type)
{
	if (domain <= 0)
	{
		domain = AF_INET;
	}
	if (type <= 0)
	{
		type = SOCK_DGRAM;
	}
	// Allocate struct
	Socket *self = (Socket *)calloc(1, sizeof(Socket));
	// Assign function pointers
	self->recvfrom = socket_recvfrom;
	self->sendto = socket_sendto;
	self->recvready = socket_recvready;
	self->bind = socket_bind;
	self->listen = socket_listen;
	self->accept = socket_accept;
	// Assign vars
	self->domain = domain;
	self->type = type;

	// Create socket
	self->socket = socket(domain, type, 0);
	if (self->socket == -1)
	{
		return NULL;
	}

	return self;
}

int socket_bind(Socket *self, char *address, int port)
{
	struct sockaddr_in *sockaddr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr->sin_port = htons(port);

        // Bind to the port
	return bind(self->socket, (struct sockaddr *) sockaddr, sizeof(struct sockaddr_in));
}

int socket_listen(Socket *self, int max_pending)
{
	return listen(self->socket, max_pending);
}

int socket_accept(Socket *self)
{
	struct sockaddr_in *sockaddr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));;
	socklen_t len = sizeof(struct sockaddr_in);
	int newsock = accept(self->socket, (struct sockaddr *)sockaddr, &len);
	// Create new Socket object here with this socket
	return newsock;
}

int socket_recvfrom(Socket *self, char *buf, int buf_size, struct sockaddr *sockaddr, unsigned int *sockaddr_size)
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

int socket_sendto(Socket *self, char *buf, struct sockaddr *sockaddr, unsigned int sockaddr_size)
{
	int n = sendto(self->socket, buf, strlen(buf), 0, sockaddr, sockaddr_size);
	if (n < 0)
	{
		printf("socket_sendto(): %s\n", strerror(errno));
	}
	return n;
}

int socket_recvready(Socket *self, int timeout_sec)
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
