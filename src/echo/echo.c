#include "common/main.h"
#include "common/hash.h"
#include "common/util.h"
#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int echo_entry(Hash * config_opts)
{
	int port, socket_type;
	char proto[5];
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
	}
	if ((tmpvalue = config_opts->get(config_opts, "proto")) != NULL)
	{
		strncpy(proto, tmpvalue, sizeof(proto));
		proto[sizeof(proto)-1] = 0;
		if (!strcmp(proto, "tcp"))
		{
			socket_type = SOCK_STREAM;
		}
		else if (!strcmp(proto, "udp"))
		{
			socket_type = SOCK_DGRAM;
		}
		else
		{
			fprintf(stderr, "Invalid proto specification '%s'. Must be one of: tcp, udp\n", tmpvalue);
			return 1;
		}
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
	}
	if ((tmpvalue = config_opts->get(config_opts, "num_workers")) != NULL)
	{
		num_workers = atoi(tmpvalue);
		if (num_workers <= 0 || num_workers > 200)
		{
			fprintf(stderr, "Invalid num_workers specification: %s\n", tmpvalue);
			return 1;
		}
	}

	Socket *sock = Socket_init(0);
	sock->create(sock, AF_INET, socket_type);
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
	if (socket_type == SOCK_STREAM)
	{
		if (sock->listen(sock, 5) < 0)
		{
			fprintf(stderr, "Listen on socket failed\n");
			return 1;
		}
	}

	Dispatcher * disp = Dispatcher_init(worker_model, num_workers);
	disp->add_listener(disp, sock, echo_dispatcher_poll_callback, echo_dispatcher_run_callback, echo_dispatcher_cleanup_callback);
	disp->run(disp);

	disp->destroy(disp);
	sock->destroy(sock);

	return 0;
}

int echo_dispatcher_poll_callback(dispatcher_callback_info * cb_info)
{
	switch (cb_info->sock->type)
	{
		case SOCK_DGRAM:
		{
			char * buf = (char *)calloc(1, ECHO_MAX_UDP_PACKET_SIZE);
			struct sockaddr * client_addr = (struct sockaddr *)calloc(1, sizeof(struct sockaddr));
			socklen_t len = sizeof(struct sockaddr);
			if (cb_info->sock->recvfrom(cb_info->sock, buf, ECHO_MAX_UDP_PACKET_SIZE - 1, client_addr, &len) <= 0)
			{
				// error in recvfrom
				return 1;
			}
			cb_info->data[0] = (void *)buf;
			cb_info->data[1] = (void *)client_addr;
			break;
		}
		case SOCK_STREAM:
			cb_info->sock = cb_info->sock->accept(cb_info->sock);
			if (cb_info->sock == NULL)
			{
				return 1;
			}
			break;
	}
	return 0;
}

int echo_dispatcher_cleanup_callback(dispatcher_callback_info * cb_info)
{
	switch (cb_info->sock->type)
	{
		case SOCK_DGRAM:
			free(cb_info->data[0]);
			free(cb_info->data[1]);
			break;
		case SOCK_STREAM:
			cb_info->sock->destroy(cb_info->sock);
			break;
	}
	return 0;
}

int echo_dispatcher_run_callback(dispatcher_callback_info * cb_info)
{
	switch (cb_info->sock->type)
	{
		case SOCK_DGRAM:
			cb_info->sock->sendto(cb_info->sock, (char *)cb_info->data[0], (struct sockaddr *)cb_info->data[1], sizeof(struct sockaddr));
			free(cb_info->data[0]);
			free(cb_info->data[1]);
			break;
		case SOCK_STREAM:
		{
			int n;
			char buf[1024], outbuf[1024];
			Socket * sock = cb_info->sock;
			while (1)
			{
				if (sock->recvready(sock, 60))
				{
					n = sock->read(sock, buf, sizeof(buf));
					if (n == -1)
					{
						printf("echo_recv_ready_tcp(): read() error: %s\n", strerror(errno));
						return 1;
					}
					if (n == 0)
					{
						printf("echo_recv_ready_tcp(): connection closed\n");
						break;
					}
					rtrim(buf);
					sprintf(outbuf, "%s\n", buf);
					sock->write(sock, outbuf, strlen(outbuf));
					buf[0] = 0;
					break;
				}
			}
		}
	}
	return 0;
}

