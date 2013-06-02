#include "server.h"

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

int server_start(type_server_info server_info)
{
	if(!strcmp(server_info.proto, "tcp"))
	{
		return server_tcp_start(server_info);
	}
	else if(!strcmp(server_info.proto, "udp"))
	{
		return server_udp_start(server_info);
	}
	else
	{
		fprintf(stderr, "Unsupported proto: %s\n", server_info.proto);
		return 1;
	}
}

int server_tcp_start(type_server_info server_info)
{
	printf("Would start TCP server on port %d\n", server_info.port);
	return 0;
}

int server_udp_start(type_server_info server_info)
{
	printf("Would start UDP server on port %d\n", server_info.port);
	return 0;
}
