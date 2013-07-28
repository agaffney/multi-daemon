#include "common/hash.h"
#include "common/socket.h"

struct _HttpRequest {
	char method[20];
	char url[1024];
	char decoded_url[1024];
	char http_version[15];
	Hash * headers;
	void (*destroy)(struct _HttpRequest *);
	int (*parse)(struct _HttpRequest *, char *);
	int (*read_from_socket)(struct _HttpRequest *, Socket *);
};

typedef struct _HttpRequest HttpRequest;

HttpRequest * HttpRequest_init();
void _http_request_destroy(HttpRequest *);
int _http_request_parse(HttpRequest *, char *);
int _http_request_read_from_socket(HttpRequest *, Socket *);
