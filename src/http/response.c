#include "response.h"
#include "common/util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const http_response_code _http_response_codes[] = {
	{ 100, "Continue" },
	{ 101, "Switching Protocols" },
	{ 200, "OK" },
	{ 201, "Created" },
	{ 202, "Accepted" },
	{ 203, "Non-Authoritative Information" },
	{ 204, "No Content" },
	{ 205, "Reset Content" },
	{ 206, "Partial Content" },
	{ 300, "Multiple Choices" },
	{ 301, "Moved Permanently" },
	{ 302, "Moved Temporarily" },
	{ 303, "See Other" },
	{ 304, "Not Modified" },
	{ 305, "Use Proxy" },
	{ 306, "Unused" },
	{ 307, "Temporary Redirect" },
	{ 400, "Bad Request" },
	{ 401, "Unauthorized" },
	{ 402, "Payment Required" },
	{ 403, "Forbidden" },
	{ 404, "Not Found" },
	{ 405, "Method Not Allowed" },
	{ 406, "Not Acceptable" },
	{ 407, "Proxy Authentication Required" },
	{ 408, "Request Time-out" },
	{ 409, "Conflict" },
	{ 410, "Gone" },
	{ 411, "Length Required" },
	{ 412, "Precondition Failed" },
	{ 413, "Request Entity Too Large" },
	{ 414, "Request-URI Too Large" },
	{ 415, "Unsupported Media Type" },
	{ 416, "Requested Range Not Satisfiable" },
	{ 417, "Expectation Failed" },
	{ 500, "Internal Server Error" },
	{ 501, "Not Implemented" },
	{ 502, "Bad Gateway" },
	{ 503, "Service Unavailable" },
	{ 504, "Gateway Time-out" },
	{ 505, "HTTP Version not supported" },
	{ 0, "" }
};

HttpResponse * _http_response_init()
{
	HttpResponse * obj = (HttpResponse *)calloc(1, sizeof(HttpResponse));
	obj->headers = _hash_init();
	obj->set_status = _http_response_set_status;
	obj->output = _http_response_output;
	return obj;
}

int _http_response_set_status(HttpResponse * self, unsigned int code)
{
	int i;
	for (i = 0;; i++)
	{
		if (_http_response_codes[i].code == 0)
		{
			return 1;
		}
		if (_http_response_codes[i].code == code)
		{
			self->_status_code = code;
			strcpy(self->_status_str, _http_response_codes[i].msg);
			return 0;
		}
	}
}

char * _http_response_output(HttpResponse * self, char * buf, int len)
{
	int i;
	char *tmpbuf = (char *)calloc(1, len);
	sprintf(tmpbuf, "HTTP/%s %d %s\r\n", self->http_version, self->_status_code, self->_status_str);
	// Output headers
	List * keys = self->headers->keys(self->headers);
	for (i = 0; i < keys->length(keys); i++)
	{
		char * key = keys->get(keys, i);
		sprintf(tmpbuf, "%s%s: %s\r\n", tmpbuf, key, self->headers->get(self->headers, key));
	}
	sprintf(tmpbuf, "%s\r\n", tmpbuf);
	strncpy(buf, tmpbuf, len - 1);
	buf[len - 1] = 0;
	free(tmpbuf);
	return buf;
}


