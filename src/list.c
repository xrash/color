#include "list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Allocate and return a new list node for the specified element. */
list_node* list_node_new(void* e) {
	list_node* n = malloc(sizeof(list_node));
	n->element = e;
	n->next = NULL;
	return n;
}

/* Allocate and return a new list. */
list* list_new() {
	list* l = malloc(sizeof(list));
	l->head = list_node_new(NULL);
	l->tail = list_node_new(NULL);
	l->length = 0;
	return l;
}

/* Create a list node from the specified element and append it to the list. */
int list_add(list* l, void* e) {
	list_node* n = list_node_new(e);

	if (l == NULL || e == NULL) {
		return 0;
	}

	if (n != NULL) {
		if (l->head->next == NULL) {
			l->head->next = n;
		}

		if (l->tail != NULL) {
			l->tail->next = n;
		}

		l->tail = n;
		l->length++;

		return 1;
	}

	return 0;
}

/* Print the list for debugging purposes. */
void list_print(list* l) {
	list_node* n = l->head;

	while ((n = n->next) != NULL) {
		printf("list_print: %p\n", n->element);
	}
}
