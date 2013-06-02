#include "main.h"
#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int echo_entry(type_server_opt server_opts[])
{
	int i, port;
	char proto[5];

	for(i=0;;i++)
	{
		if (!strcmp(server_opts[i].name, ""))
		{
			// End of options
			break;
		}
		if (!strcmp(server_opts[i].name, "port"))
		{
			port = atoi(server_opts[i].value);
			if (port <= 0 || port >= 65535)
			{
				fprintf(stderr, "Invalid port specification: %s\n", server_opts[i].value);
				return 1;
			}
		}
		else if (!strcmp(server_opts[i].name, "proto"))
		{
			strncpy(proto, server_opts[i].value, sizeof(proto));
			proto[sizeof(proto)-1] = 0;
			if (strcmp(proto, "tcp") && strcmp(proto, "udp"))
			{
				fprintf(stderr, "Invalid proto specification '%s'. Must be one of: tcp, udp\n", server_opts[i].value);
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Unsupported option: %s\n", server_opts[i].name);
			return 1;
		}
	}
	printf("echo_entry(): would start server on %s/%d\n", proto, port);

	return 0;
}
