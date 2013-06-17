#include "socket.h"
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
