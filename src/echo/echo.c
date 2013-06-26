#include "common/config.h"
#include "common/main.h"
#include "common/hash.h"
#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int echo_entry(config_opt config_opts[])
{
	int i, port;
	char proto[5];
	int (*recv_ready_callback)();

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
				recv_ready_callback = echo_recv_ready_tcp;
			}
			else if (!strcmp(proto, "udp"))
			{
				recv_ready_callback = echo_recv_ready_udp;
			}
			else
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

	server_info *srv_info = (server_info *)calloc(1, sizeof(server_info));
	strcpy(srv_info->proto, proto);
	srv_info->port = port;
	srv_info->recv_ready_callback = recv_ready_callback;

	i = server_start(srv_info);

	free(srv_info);

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
	Hash * my_hash = _hash_init();

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
			while (1)
			{
				if (isspace(buf[strlen(buf)-1]))
				{
					buf[strlen(buf)-1] = 0;
					continue;
				}
				break;
			}
			if (!strcmp(buf, "GIMME"))
			{
				int i;
				List * my_list = my_hash->keys(my_hash);
				for (i = 0; i < my_list->length(my_list); i++)
				{
					sprintf(outbuf, "%s: %s\n", my_list->get(my_list, i), my_hash->get(my_hash, my_list->get(my_list, i)));
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
