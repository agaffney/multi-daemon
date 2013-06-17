#include "socket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int socket_init(Socket *self, int socket)
{
	self->socket = socket;
	self->recvfrom = socket_recvfrom;
	self->sendto = socket_sendto;
	self->recvready = socket_recvready;
	return 0;
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
