#include "hash.h"

#include <stdlib.h>
#include <string.h>

Hash * Hash_init()
{
	Hash * hashobj = (Hash *)calloc(1, sizeof(Hash));
	hashobj->_keys = List_init();
	hashobj->destroy = _hash_destroy;
	hashobj->get = _hash_get;
	hashobj->set = _hash_set;
	hashobj->unset = _hash_unset;
	hashobj->has_key = _hash_has_key;
	hashobj->keys = _hash_keys;
	int i;
	for (i = 0; i < _HASH_TABLE_SIZE; i++)
	{
		hashobj->_table[i] = List_init();	
	}
	return hashobj;
}

void _hash_destroy(Hash * self)
{
	self->_keys->destroy(self->_keys);
	int i;
	for (i = 0; i < _HASH_TABLE_SIZE; i++)
	{
		self->_table[i]->destroy(self->_table[i]);
	}
	free(self);
}

/*
DJB hashing function
Modified from http://partow.net/programming/hashfunctions/index.html
*/
unsigned int _hash_hash(char * str)
{
	unsigned int hash = 5381;
	unsigned int i    = 0;

	unsigned int len = strlen(str);

	for(i = 0; i < len; str++, i++)
	{
		hash = ((hash << 5) + hash) + (*str);
	}

	return hash;
}

void _hash_set(Hash * self, char * key, void * value, int type)
{
	unsigned int hash = _hash_hash(key) % _HASH_TABLE_SIZE;
	List * tmp_list = self->_table[hash];
	if (self->has_key(self, key))
	{
		int i;
		for (i = 0; i < tmp_list->item_count; i = i + 2)
		{
			if (!strcmp(tmp_list->get(tmp_list, i), key))
			{
				tmp_list->set(tmp_list, i + 1, value, type);
				break;
			}
		}
	}
	else
	{
		tmp_list->push(tmp_list, key, LIST_TYPE_STRING);
		tmp_list->push(tmp_list, value, type);
		self->_keys->push(self->_keys, key, LIST_TYPE_STRING);
	}
}

void _hash_unset(Hash * self, char * key)
{
	unsigned int hash = _hash_hash(key) % _HASH_TABLE_SIZE;
	List * tmp_list = self->_table[hash];
	int i;
	for (i = 0; i < tmp_list->item_count; i = i + 2)
	{
		if (!strcmp(tmp_list->get(tmp_list, i), key))
		{
			tmp_list->pop(tmp_list, i);
			tmp_list->pop(tmp_list, i);
			// Find and remove from keys
			for (int j = 0; j < self->_keys->item_count; j++)
			{
				if (!strcmp(self->_keys->get(self->_keys, j), key))
				{
					self->_keys->pop(self->_keys, j);
					break;
				}
			}
			break;
		}
	}
}

char * _hash_get(Hash * self, char * key)
{
	unsigned int hash = _hash_hash(key) % _HASH_TABLE_SIZE;
	List * tmp_list = self->_table[hash];
	int i;
	for (i = 0; i < tmp_list->item_count; i = i + 2)
	{
		if (!strcmp(tmp_list->get(tmp_list, i), key))
		{
			return tmp_list->get(tmp_list, i + 1);
		}
	}
	return NULL;
}

int _hash_has_key(Hash * self, char * key)
{
	int i;
	for (i = 0; i < self->_keys->item_count; i++)
	{
		if (!strcmp(self->_keys->get(self->_keys, i), key))
		{
			return 1;
		}
	}
	return 0;
}

List * _hash_keys(Hash * self)
{
	return self->_keys;
}
