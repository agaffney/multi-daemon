#include "list.h"

#include <stdlib.h>
#include <string.h>

List * List_init()
{
	List * listobj = (List *)calloc(1, sizeof(List));
	if (listobj == NULL)
	{
		return NULL;
	}
	listobj->destroy = _list_destroy;
	listobj->push = _list_push;
	listobj->pop = _list_pop;
	listobj->get = _list_get;
	listobj->set = _list_set;
	listobj->length = _list_length;
	return listobj;
}

void _list_destroy(List * self)
{
	int i;
	ListItem * tmp_item = self->items;
	ListItem * tmp_item2;
	for (i = 0; i < self->item_count; i++)
	{
		tmp_item2 = tmp_item;
		// Move to next link in the chain
		tmp_item = tmp_item->next_item;
		// Free memory for current link in the chain
		if (tmp_item2->type == LIST_TYPE_STRING)
		{
			free(tmp_item2->value);
		}
		free(tmp_item2);
	}
	free(self);
}

char * _list_pop(List * self, int index)
{
	if (index > self->item_count - 1)
	{
		return NULL;
	}
	if (index == -1)
	{
		index = self->item_count - 1;
	}
	int i;
	ListItem * tmp_item = self->items;
	ListItem * prev_item = NULL;
	for (i = 0; i < index; i++)
	{
		prev_item = tmp_item;
		tmp_item = tmp_item->next_item;
	}
	// Now that we found it, remove it
	if (index == 0)
	{
		self->items = tmp_item->next_item;
	}
	else
	{
		prev_item->next_item = tmp_item->next_item;
	}
	self->item_count--;
	return tmp_item->value;
}

char * _list_get(List * self, int index)
{
	if (index > self->item_count - 1)
	{
		return NULL;
	}
	int i;
	ListItem * tmp_item = self->items;
	for (i = 0; i < index; i++)
	{
		tmp_item = tmp_item->next_item;
	}
	return tmp_item->value;
}

void _list_set(List * self, int index, void * value, int type)
{
	int i;
	ListItem * tmp_item = self->items;
	for (i = 0; i < index; i++)
	{
		tmp_item = tmp_item->next_item;
	}
	if (tmp_item->value != NULL)
	{
		// Free memory for old value if we allocated it
		if (tmp_item->type == LIST_TYPE_STRING)
		{
			free(tmp_item->value);
		}
	}
	switch (type)
	{
		case LIST_TYPE_STRING:
			tmp_item->value = (char *)calloc(1, strlen(value) + 1);
			strcpy(tmp_item->value, value);
			break;
		case LIST_TYPE_POINTER:
			tmp_item->value = value;
			break;
	}
	tmp_item->type = type;
}

void _list_push(List * self, void * value, int type)
{
	ListItem * new_item = (ListItem *)calloc(1, sizeof(ListItem));
	if (self->item_count == 0)
	{
		self->items = new_item;
	}
	else
	{
		int i;
		ListItem * tmp_item = self->items;
		for (i = 0; i < self->item_count - 1; i++)
		{
			tmp_item = tmp_item->next_item;
		}
		tmp_item->next_item = new_item;
	}
	self->item_count++;
	_list_set(self, self->item_count - 1, value, type);
}

int _list_length(List * self)
{
	return self->item_count;
}
