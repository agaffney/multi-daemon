#include "common/hash.h"

struct _HttpRequest {
	char method[20];
	char url[1024];
	char http_version[15];
	Hash * headers;
	int (*parse)(struct _HttpRequest *, char *);
};

typedef struct _HttpRequest HttpRequest;

HttpRequest * HttpRequest_init();
int _http_request_parse(HttpRequest *, char *);
