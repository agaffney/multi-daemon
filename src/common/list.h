#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

struct _ListItem {
	char * value;
	struct _ListItem * next_item;
};

typedef struct _ListItem ListItem;

struct _List {
	ListItem * items;
	int item_count;
	void (*destroy)(struct _List *);
	char * (*get)(struct _List *, int);
	void (*set)(struct _List *, int, char *);
	void (*push)(struct _List *, char *);
	int (*length)(struct _List *);
};

typedef struct _List List;

List * List_init();
void _list_destroy();
char * _list_get(List *, int);
void _list_set(List *, int, char *);
void _list_push(List *, char *);
int _list_length(List *);

#endif
