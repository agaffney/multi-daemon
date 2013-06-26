#include "common/config.h"
#include "common/main.h"
#include "http.h"
#include "request.h"

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
	char outbuf[1024];
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
			HttpRequest * req = _http_request_init();
			req->parse(req, buf);
			sprintf(outbuf, "HTTP/%s 200 OK\r\nContent-length: 0\r\n\r\n", req->http_version);
			sock->write(sock, outbuf, strlen(outbuf));
			buf[0] = 0;
			sock->close(sock);
			return 0;
		}
	}

	return 0;
}
