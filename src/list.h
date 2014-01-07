#ifndef LIST_H
#define LIST_H

typedef struct list_node_ {
	void* element;
	struct list_node_* next;
} list_node;

typedef struct {
	list_node* head;
	list_node* tail;
	int length;
} list;

extern list_node* list_node_new(void* e);
extern list* list_new();
extern int list_add(list* l, void* e);
extern void list_print(list* l);

#endif /* LIST_H */
