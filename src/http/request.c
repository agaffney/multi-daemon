#include "request.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

HttpRequest * _http_request_init()
{
	HttpRequest * obj = (HttpRequest *)calloc(1, sizeof(HttpRequest));
	obj->headers = _hash_init();
	obj->parse = _http_request_parse;
	return obj;
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
				return 0;
			}
			return 1;
		}
		if (tmp2[1] != '\n')
		{
			// Line does not end in \r\n
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
				return 0;
			}
			printf("Request: %s %s %s\n", self->method, self->url, self->http_version);
		}
		else
		{
			// Header
			char * key;
			char * value;
			key = strtok(line, ": ");
			if (key == NULL)
			{
				free(line);
				return 0;
			}
			value = strtok(NULL, "");
			if (value == NULL)
			{
				free(line);
				return 0;
			}
			self->headers->set(self->headers, key, value + 1);
			printf("Header: '%s' = '%s'\n", key, value + 1);
		}
		free(line);
	}

	return 1;
}
