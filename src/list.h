#ifndef __LIST__H_
#define __LIST__H_

// simple stack, holds generic data

struct ListItem {
	void *data;
	struct ListItem *next;
};
typedef struct ListItem ListItem;

typedef struct {
	ListItem *head;
} List;

List *List_create();                                           // for heap list
void _List_delete(List *list, void (*deleteData)(void *data)); // for heap list
void _List_clear(List *list, void (*deleteData)(void *data));  // both
bool _List_add(List *list, void *data);                        // both
void *List_pop(List *list);                                    // both

#define List_delete(l, f) _List_delete((l), (void (*)(void *data))(f));
#define List_clear(l, f) _List_clear((l), (void (*)(void *data))(f));
#define List_add(l, d) _List_add((l), (void *)(d));

#endif
