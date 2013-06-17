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

	// Create socket
	Socket *sock = socket_init(AF_INET, SOCK_DGRAM);
	if (sock == NULL)
	{
		fprintf(stderr, "Failed to create socket\n");
		return 1;
	}

	// Bind to the port
	sock->bind(sock, "0.0.0.0", server_info->port);

	if (server_info->recv_ready_callback != NULL)
	{
		(*server_info->recv_ready_callback)(sock);
	}

	return 0;
}
