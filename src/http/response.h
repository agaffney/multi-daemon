#include "common/hash.h"

struct _http_response {
	unsigned int _status_code;
	char _status_str[256];
	char http_version[6];
	Hash * headers;
	int (*set_status)(struct _http_response *, unsigned int);
	char * (*output)(struct _http_response *, char *, int);
};

typedef struct _http_response HttpResponse;

struct _http_response_code {
	unsigned int code;
	char msg[256];
};

typedef struct _http_response_code http_response_code;

HttpResponse * _http_response_init();
int _http_response_set_status(HttpResponse *, unsigned int);
char * _http_response_output(HttpResponse *, char *, int);
