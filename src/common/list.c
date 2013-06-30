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
		free(tmp_item2->value);
		free(tmp_item2);
	}
	free(self);
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

void _list_set(List * self, int index, char * value)
{
	int i;
	ListItem * tmp_item = self->items;
	for (i = 0; i < index; i++)
	{
		tmp_item = tmp_item->next_item;
	}
	if (tmp_item->value != NULL)
	{
		// Free memory for old value
		free(tmp_item->value);
	}
	tmp_item->value = (char *)calloc(1, strlen(value) + 1);
	strcpy(tmp_item->value, value);
}

void _list_push(List * self, char * value)
{
	ListItem * new_item = (ListItem *)calloc(1, sizeof(ListItem));
	new_item->value = (char *)calloc(1, strlen(value) + 1);
	strcpy(new_item->value, value);
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
}

int _list_length(List * self)
{
	return self->item_count;
}
