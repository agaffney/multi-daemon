#include "common/hash.h"

struct _HttpResponse {
	unsigned int _status_code;
	char _status_str[256];
	char http_version[6];
	Hash * headers;
	void (*destroy)(struct _HttpResponse *);
	int (*set_status)(struct _HttpResponse *, unsigned int);
	char * (*output)(struct _HttpResponse *, char *, int);
};

typedef struct _HttpResponse HttpResponse;

struct _http_response_code {
	unsigned int code;
	char msg[256];
};

typedef struct _http_response_code http_response_code;

HttpResponse * HttpResponse_init();
void _http_response_destroy(HttpResponse *);
int _http_response_set_status(HttpResponse *, unsigned int);
char * _http_response_output(HttpResponse *, char *, int);
