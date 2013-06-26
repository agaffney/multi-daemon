#include "list.h"

#define _HASH_TABLE_SIZE 256

struct _hash {
	List * _table[_HASH_TABLE_SIZE];
	List * _keys;
	char * (*get)(struct _hash *, char *);
	void (*set)(struct _hash *, char *, char *);
	int (*has_key)(struct _hash *, char *);
	List * (*keys)(struct _hash *);
};

typedef struct _hash Hash;

Hash * _hash_init();
unsigned int _hash_hash(char *);
void _hash_set(Hash *, char *, char *);
char * _hash_get(Hash *, char *);
int _hash_has_key(Hash *, char *);
List * _hash_keys(Hash *);
