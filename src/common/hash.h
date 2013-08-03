#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "list.h"

#define _HASH_TABLE_SIZE 256

struct _Hash {
	List * _table[_HASH_TABLE_SIZE];
	List * _keys;
	void (*destroy)(struct _Hash *);
	char * (*get)(struct _Hash *, char *);
	void (*set)(struct _Hash *, char *, char *);
	void (*unset)(struct _Hash *, char *);
	int (*has_key)(struct _Hash *, char *);
	List * (*keys)(struct _Hash *);
};

typedef struct _Hash Hash;

Hash * Hash_init();
void _hash_destroy();
unsigned int _hash_hash(char *);
void _hash_set(Hash *, char *, char *);
void _hash_unset(Hash *, char *);
char * _hash_get(Hash *, char *);
int _hash_has_key(Hash *, char *);
List * _hash_keys(Hash *);

#endif
