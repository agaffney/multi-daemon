#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#define LIST_TYPE_STRING  1
#define LIST_TYPE_POINTER 2

struct _ListItem {
	char * value;
	int type;
	struct _ListItem * next_item;
};

typedef struct _ListItem ListItem;

struct _List {
	ListItem * items;
	int item_count;
	void (*destroy)(struct _List *);
	char * (*get)(struct _List *, int);
	char * (*pop)(struct _List *, int);
	void (*set)(struct _List *, int, void *, int);
	void (*push)(struct _List *, void *, int);
	int (*length)(struct _List *);
};

typedef struct _List List;

List * List_init();
void _list_destroy();
char * _list_get(List *, int);
char * _list_pop(List *, int);
void _list_set(List *, int, void *, int);
void _list_push(List *, void *, int);
int _list_length(List *);

#endif
