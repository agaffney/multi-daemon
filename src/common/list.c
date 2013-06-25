#include "list.h"

#include <stdlib.h>
#include <string.h>

List * _list_init()
{
	List * listobj = (List *)calloc(1, sizeof(List));
	if (listobj == NULL)
	{
		return NULL;
	}
	listobj->push = _list_push;
	listobj->get = _list_get;
	listobj->length = _list_length;
	return listobj;
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

void _list_push(List * self, char * value)
{
	ListItem * new_item = (ListItem *)calloc(1, sizeof(ListItem));
	new_item->value = (char *)calloc(1, strlen(value));
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
