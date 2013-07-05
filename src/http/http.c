#include "common/config.h"
#include "common/main.h"
#include "http.h"
#include "request.h"
#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int http_entry(config_opt config_opts[])
{
	int port;

	for(int i = 0;; i++)
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

	Socket *sock = Socket_init(0);
	sock->create(sock, AF_INET, SOCK_STREAM);
	if (sock == NULL)
	{
		fprintf(stderr, "Failed to create socket\n");
		return 1;
	}

	sock->set_flag(sock, O_NONBLOCK);
	if (sock->bind(sock, "0.0.0.0", port) < 0)
	{
		fprintf(stderr, "Bind to socket failed\n");
		return 1;
	}
	if (sock->listen(sock, 5) < 0)
	{
		fprintf(stderr, "Listen on socket failed\n");
		return 1;
	}

	Dispatcher * disp = Dispatcher_init(DISPATCHER_WORKER_MODEL_THREAD, 5);
	disp->add_listener(disp, sock, http_dispatcher_callback);
	disp->run(disp);

	disp->destroy(disp);
	sock->destroy(sock);

	return 0;
}

int http_dispatcher_callback(Dispatcher * dispatcher, Socket * sock)
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
			HttpRequest * req = HttpRequest_init();
			req->parse(req, buf);
			HttpResponse * resp = HttpResponse_init();
			strcpy(resp->http_version, req->http_version);
			resp->set_status(resp, 200);
			resp->headers->set(resp->headers, "Content-length", "0");
			resp->output(resp, outbuf, sizeof(outbuf));
			sock->write(sock, outbuf, strlen(outbuf));
			req->destroy(req);
			resp->destroy(resp);
			break;
		}
	}
	if (sock->close(sock) < 0)
	{
		fprintf(stderr, "http_accept(): close() on socket failed\n");
	}

	return 0;
}
