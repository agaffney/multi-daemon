#include "config.h"
#include "main.h"
#include "server.h"
#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int echo_entry(config_opt config_opts[])
{
	int i, port;
	char proto[5];

	for(i=0;;i++)
	{
		if (!strcmp(config_opts[i].name, ""))
		{
			// End of options
			break;
		}
		if (!strcmp(config_opts[i].name, "port"))
		{
			port = atoi(config_opts[i].value);
			if (port <= 0 || port >= 65535)
			{
				fprintf(stderr, "Invalid port specification: %s\n", config_opts[i].value);
				return 1;
			}
		}
		else if (!strcmp(config_opts[i].name, "proto"))
		{
			strncpy(proto, config_opts[i].value, sizeof(proto));
			proto[sizeof(proto)-1] = 0;
			if (strcmp(proto, "tcp") && strcmp(proto, "udp"))
			{
				fprintf(stderr, "Invalid proto specification '%s'. Must be one of: tcp, udp\n", config_opts[i].value);
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Unsupported option: %s\n", config_opts[i].name);
			return 1;
		}
	}

	type_server_info server_info;
	memset(&server_info, 0, sizeof(server_info));
	strcpy(server_info.proto, proto);
	server_info.port = port;
	server_info.recv_callback = echo_recv;

	i = server_start(server_info);

	return 0;
}

int echo_recv(char *msg, void *callback_info)
{
	type_server_callback_info *cb_info = (type_server_callback_info *)callback_info;
	printf("Received from %s:%d, message: %s\n", cb_info->client_ip, cb_info->client_port, msg);
	server_sendto(msg, cb_info);
	return 0;
}
