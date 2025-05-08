#include "list.h"
#include "alloc.h"
#include <SDL3/SDL_log.h>
#include <assert.h>

List *List_create() {
	List *list = ALLOC_ZERO(1, *list);
	CHECK_ALLOC(list, NULL);
	return list;
}

void _List_delete(List *list, void (*deleteData)(void *data)) {
	List_clear(list, deleteData);
	FREE(list);
}

void _List_clear(List *list, void (*deleteData)(void *data)) {
	assert(list != NULL);

	void *data;
	while ((data = List_pop(list)) != NULL) {
		if (deleteData)
			(*deleteData)(data);
	}
}

bool _List_add(List *list, void *data) {
	assert(list != NULL);
	ListItem *item = ALLOC_ZERO(1, *item);
	CHECK_ALLOC(item, false);
	item->data = data;
	item->next = list->head;
	list->head = item;
	return true;
}

void *List_pop(List *list) {
	assert(list != NULL);
	if (list->head == NULL)
		return NULL;
	void *data = list->head->data;
	ListItem *tmp = list->head->next;
	FREE(list->head);
	list->head = tmp;
	return data;
}
