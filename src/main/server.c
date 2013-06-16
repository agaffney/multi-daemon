#include "server.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int server_start(server_info *server_info)
{
	if(!strcmp(server_info->proto, "tcp"))
	{
		return server_tcp_start(server_info);
	}
	else if(!strcmp(server_info->proto, "udp"))
	{
		return server_udp_start(server_info);
	}
	else
	{
		fprintf(stderr, "Unsupported proto: %s\n", server_info->proto);
		return 1;
	}
}

int server_tcp_start(server_info *server_info)
{
	printf("Would start TCP server on port %d\n", server_info->port);
	return 0;
}

int server_udp_start(server_info *server_info)
{
	printf("Starting UDP server on port %d\n", server_info->port);

	int socket_fd;
	struct sockaddr_in server_addr;

	// Create socket
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	// Populate struct with proto/port info
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(server_info->port);

	// Bind to the port
	bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	Socket *sockobj = (Socket *)calloc(1, sizeof(Socket));
	sockobj->init = socket_init;
	sockobj->init(sockobj, socket_fd);

	if (server_info->recv_ready_callback != NULL)
	{
		(*server_info->recv_ready_callback)(sockobj);
	}

	return 0;
}

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
