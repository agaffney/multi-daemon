#include "hash.h"

typedef struct {
	const char *service;
	int (*func)(Hash *);
} dispatch_table_entry;

