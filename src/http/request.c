#include "request.h"
#include "common/util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

HttpRequest * HttpRequest_init()
{
	HttpRequest * self = (HttpRequest *)calloc(1, sizeof(HttpRequest));
	self->headers = Hash_init();
	self->destroy = _http_request_destroy;
	self->parse = _http_request_parse;
	self->read_from_socket = _http_request_read_from_socket;
	return self;
}

void _http_request_destroy(HttpRequest * self)
{
	self->headers->destroy(self->headers);
	free(self);
}

int _http_request_parse(HttpRequest * self, char * buf)
{
	int linenum;
	char * tmp = buf;
	char * tmp2;
	char * line;
	for (linenum = 1;; linenum++)
	{
		tmp2 = strchr(tmp, '\r');
		if (tmp2 == NULL)
		{
			if (linenum == 1)
			{
				printf("parse(): can't find \\r on line 1\n");
				return 0;
			}
			return 1;
		}
		if (tmp2[1] != '\n')
		{
			// Line does not end in \r\n
			printf("parse(): line %d doesn't end in \\r\\n\n", linenum);
			return 0;
		}
		line = (char *)calloc(1, (tmp2 - tmp) + 1);
		strncpy(line, tmp, (tmp2 - tmp)); 
		// Increment pointer past \r\n
		tmp = tmp2 + 2;
		if (linenum == 1)
		{
			// Request line
			if (sscanf(line, "%s %1023s HTTP/%3[0-9.]", self->method, self->url, self->http_version) < 3)
			{
				free(line);
				printf("parse(): can't parse line 1 request line\n");
				return 0;
			}
			if (urldecode(self->decoded_url, self->url))
			{
				printf("parse(): can't decode URL\n");
				return 0;
			}
//			printf("Request: %s %s (%s) %s\n", self->method, self->url, self->decoded_url, self->http_version);
		}
		else
		{
			// Header
			if (line[0] == 0)
			{
				// end of request
				free(line);
				break;
			}
			char key[100];
			char value[1024];
			if (sscanf(line, "%99[^:]: %1023s", key, value) < 2)
			{
				printf("parse(): failed to parse header on line %d\n", linenum);
				free(line);
				return 0;
			}
			ltrim(value);
			self->headers->set(self->headers, key, value);
//			printf("Header: '%s' = '%s'\n", key, value);
		}
		free(line);
	}

	return 1;
}

int _http_request_read_from_socket(HttpRequest * self, Socket * sock)
{
	char tmpbuf[4096];
	int offset = 0;
	int n;
	while (1)
	{
		n = sock->read(sock, &tmpbuf[offset], 1);
		if (n == -1)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			printf("_read_from_socket(): read() error: %s\n", strerror(errno));
			break;
		}
		if (n == 0)
		{
			sock->close(sock);
			break;
		}
		offset += n;
		tmpbuf[offset] = 0;
		if (offset >= 4 && tmpbuf[offset-4] == '\r' && tmpbuf[offset-3] == '\n' && tmpbuf[offset-2] == '\r' && tmpbuf[offset-1] == '\n')
		{
			// It looks like we got the entire request, not including the body
			int ret = self->parse(self, tmpbuf);
//			printf("_read_from_socket(): parse returned %d\n", ret);
			return ret;
		}
	}
	return 0;
}
