#include "common/main.h"
#include "http.h"
#include "request.h"
#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int http_entry(Hash * config_opts)
{
	int port;
	int worker_model = DISPATCHER_WORKER_MODEL_SINGLE;
	int num_workers = 5;
	char * tmpvalue;

	if ((tmpvalue = config_opts->get(config_opts, "port")) != NULL)
	{
		port = atoi(tmpvalue);
		if (port <= 0 || port >= 65535)
		{
			fprintf(stderr, "Invalid port specification: %s\n", tmpvalue);
			return 1;
		}
		config_opts->unset(config_opts, "port");
	}
	if ((tmpvalue = config_opts->get(config_opts, "worker_model")) != NULL)
	{
		if (!strcmp(tmpvalue, "single"))
		{
			worker_model = DISPATCHER_WORKER_MODEL_SINGLE;
		}
		else if (!strcmp(tmpvalue, "postfork"))
		{
			worker_model = DISPATCHER_WORKER_MODEL_POSTFORK;
		}
		else if (!strcmp(tmpvalue, "prefork"))
		{
			worker_model = DISPATCHER_WORKER_MODEL_PREFORK;
		}
		else if (!strcmp(tmpvalue, "thread"))
		{
			worker_model = DISPATCHER_WORKER_MODEL_THREAD;
		}
		else
		{
			fprintf(stderr, "Invalid worker_model specification: %s\n", tmpvalue);
			return 1;
		}
		config_opts->unset(config_opts, "worker_model");
	}
	if ((tmpvalue = config_opts->get(config_opts, "num_workers")) != NULL)
	{
		num_workers = atoi(tmpvalue);
		if (num_workers <= 0 || num_workers > 200)
		{
			fprintf(stderr, "Invalid num_workers specification: %s\n", tmpvalue);
			return 1;
		}
		config_opts->unset(config_opts, "num_workers");
	}
	List * remaining_keys = config_opts->keys(config_opts);
	if (remaining_keys->length(remaining_keys) > 0)
	{
		fprintf(stderr, "Unrecognized options:\n");
		for (int i = 0; i < remaining_keys->length(remaining_keys); i++)
		{
			fprintf(stderr, "%s\n", remaining_keys->get(remaining_keys, i));
		}
		return 1;
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

	Dispatcher * disp = Dispatcher_init(worker_model, num_workers);
	disp->add_listener(disp, sock, http_dispatcher_poll_callback, http_dispatcher_run_callback, http_dispatcher_cleanup_callback);
	disp->run(disp);

	disp->destroy(disp);
	sock->destroy(sock);

	return 0;
}

int http_dispatcher_poll_callback(dispatcher_callback_info * cb_info)
{
	Socket * newsock = cb_info->sock->accept(cb_info->sock);
	if (newsock == NULL)
	{
		return 1;
	}
	cb_info->sock = newsock;
	return 0;
}

int http_dispatcher_cleanup_callback(dispatcher_callback_info * cb_info)
{
	if (cb_info->extra_flag == 1)
	{
		// Parent process with POSTFORK model
		cb_info->sock->close(cb_info->sock);
	}
	cb_info->sock->destroy(cb_info->sock);
	return 0;
}

int http_dispatcher_run_callback(dispatcher_callback_info * cb_info)
{
	char outbuf[1024];

	Socket * sock = cb_info->sock;

	while (1)
	{
		HttpRequest * req = HttpRequest_init();
		if (!req->read_from_socket(req, sock))
		{
			// An error occured while reading from the socket or parsing the request
			break;
		}
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
	if (sock->close(sock) < 0)
	{
		fprintf(stderr, "http_accept(): close() on socket failed\n");
	}

	return 0;
}
