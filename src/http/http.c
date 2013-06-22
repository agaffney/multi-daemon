#include "config.h"
#include "main.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int http_entry(config_opt config_opts[])
{
	int i, port;

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
		else
		{
			fprintf(stderr, "Unsupported option: %s\n", config_opts[i].name);
			return 1;
		}
	}

	server_info *srv_info = (server_info *)calloc(1, sizeof(server_info));
	strcpy(srv_info->proto, "tcp");
	srv_info->port = port;
	srv_info->recv_ready_callback = http_accept;

	i = server_start(srv_info);

	free(srv_info);

	return 0;
}

int http_accept(Socket *sock)
{
	char buf[1024];
	int n;

	while (1)
	{
		if (sock->recvready(sock, 60))
		{
			n = sock->read(sock, buf, sizeof(buf));
			if (n == -1)
			{
				printf("http_accept(): read() error: %s\n", strerror(errno));
				break;
			}
			if (n == 0)
			{
				printf("http_accept(): connection closed\n");
				break;
			}
			printf("buf = '%s'\n", buf);
			sock->write(sock, buf, strlen(buf));
			buf[0] = 0;
		}
	}

	return 0;
}
