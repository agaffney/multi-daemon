#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

	int socket_fd, n;
	struct sockaddr_in server_addr, client_addr;
	socklen_t len;
	char buf[1000];

	// Create socket
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	// Populate struct with proto/port info
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(server_info->port);

	// Bind to the port
	bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if (server_info->recv_ready_callback != NULL)
	{
		(*server_info->recv_ready_callback)();
	}
	else
	{
		while (1)
		{
			len = sizeof(client_addr);
			n = recvfrom(socket_fd, buf, 1000, 0, (struct sockaddr *) &client_addr, &len);
			buf[n] = 0;
			if (server_info->recv_callback != NULL)
			{
				server_callback_info *callback_info = (server_callback_info *)calloc(1, sizeof(server_callback_info));
				callback_info->server_info = server_info;
				callback_info->client_addr = (struct sockaddr *)&client_addr;
				callback_info->socket_fd = socket_fd;
				callback_info->client_port = ntohs(client_addr.sin_port);
				inet_ntop(AF_INET, &client_addr.sin_addr, callback_info->client_ip, sizeof(callback_info->client_ip));

				// Call the callback function
				(*server_info->recv_callback)(buf, (void *)callback_info);

				// Free the struct pointer
				free(callback_info);
			}
		}
	}

	return 0;
}

int server_sendto(char *msg, server_callback_info *callback_info)
{
	return sendto(callback_info->socket_fd, msg, strlen(msg), 0, callback_info->client_addr, sizeof(*(callback_info->client_addr)));
}
