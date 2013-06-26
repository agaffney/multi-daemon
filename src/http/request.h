#include "common/hash.h"

struct _http_request {
	char method[20];
	char url[1024];
	char http_version[15];
	Hash * headers;
	int (*parse)(struct _http_request *, char *);
};

typedef struct _http_request HttpRequest;

HttpRequest * _http_request_init();
int _http_request_parse(HttpRequest *, char *);
