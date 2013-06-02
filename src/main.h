
typedef struct server_opt {
	char name[50];
	char value[255];
} type_server_opt;

struct dispatch_table_entry {
	const char *service;
	int (*func)(type_server_opt[]);
};

