#include "common/config.h"
#include "common/main.h"
#include "common/hash.h"
#include "common/util.h"
#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int echo_entry(config_opt config_opts[])
{
	int i, port, socket_type, worker_model, num_workers;
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
				fprintf(stderr, "Invalid proto specification '%s'. Must be one of: tcp, udp\n", config_opts[i].value);
				return 1;
			}
		}
		else if (!strcmp(config_opts[i].name, "worker_model"))
		{
			if (!strcmp(config_opts[i].value, "single"))
			{
				worker_model = DISPATCHER_WORKER_MODEL_SINGLE;
			}
			else if (!strcmp(config_opts[i].value, "postfork"))
			{
				worker_model = DISPATCHER_WORKER_MODEL_POSTFORK;
			}
			else if (!strcmp(config_opts[i].value, "prefork"))
			{
				worker_model = DISPATCHER_WORKER_MODEL_PREFORK;
			}
			else if (!strcmp(config_opts[i].value, "thread"))
			{
				worker_model = DISPATCHER_WORKER_MODEL_THREAD;
			}
			else
			{
				fprintf(stderr, "Invalid worker_model specification: %s\n", config_opts[i].value);
				return 1;
			}
		}
		else if (!strcmp(config_opts[i].name, "num_workers"))
		{
			num_workers = atoi(config_opts[i].value);
			if (num_workers <= 0 || num_workers > 200)
			{
				fprintf(stderr, "Invalid num_workers specification: %s\n", config_opts[i].value);
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
	disp->add_listener(disp, sock, echo_dispatcher_callback);
	disp->run(disp);

	disp->destroy(disp);
	sock->destroy(sock);

	return 0;
}

int echo_dispatcher_callback(Dispatcher * dispatcher, Socket * sock)
{
	switch (sock->type)
	{
		case SOCK_DGRAM:
			return echo_recv_ready_udp(sock);
			break;
		case SOCK_STREAM:
			return echo_recv_ready_tcp(sock);
			break;
	}
	return 0;
}

int echo_recv_ready_udp(Socket *sock)
{
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	char buf[1024];

	while (1)
	{
		if (sock->recvready(sock, 0))
		{
			sock->recvfrom(sock, buf, sizeof(buf), (struct sockaddr *) &client_addr, &len);
			printf("echo_recv_ready(): received string '%s'\n", buf);
			sock->sendto(sock, buf, (struct sockaddr *) &client_addr, len);
		}
	}

}

int echo_recv_ready_tcp(Socket *sock)
{
	char buf[1024];
	char outbuf[1024];
	int n;
	Hash * my_hash = Hash_init();

	while (1)
	{
		if (sock->recvready(sock, 60))
		{
			n = sock->read(sock, buf, sizeof(buf));
			if (n == -1)
			{
				printf("echo_recv_ready_tcp(): read() error: %s\n", strerror(errno));
				break;
			}
			if (n == 0)
			{
				printf("echo_recv_ready_tcp(): connection closed\n");
				break;
			}
			rtrim(buf);
			if (!strcmp(buf, "GIMME"))
			{
				int i;
				List * keys = my_hash->keys(my_hash);
				for (i = 0; i < keys->length(keys); i++)
				{
					char * key = keys->get(keys, i);
					sprintf(outbuf, "%s: %s\n", keys->get(keys, i), my_hash->get(my_hash, key));
					sock->write(sock, outbuf, strlen(outbuf));
				}
			}
			else
			{
				int val = 1;
				if (my_hash->has_key(my_hash, buf))
				{
					val = atoi(my_hash->get(my_hash, buf)) + 1;
				}
				char valbuf[10];
				sprintf(valbuf, "%d", val);
				my_hash->set(my_hash, buf, valbuf);
				sprintf(outbuf, "%s\n", buf);
				sock->write(sock, outbuf, strlen(outbuf));
			}
			buf[0] = 0;
		}
	}

	return 0;
}
