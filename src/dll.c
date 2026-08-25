//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"
#include "file-system.h"

//FUNCTIONS THAT HANDLE A DOUBLY LINKED LIST

//function that creates a doubly linked list
dll_t *dll_create(unsigned int data_size)
{
	dll_t *new_list = malloc(sizeof(dll_t));
	if (new_list == NULL) {
		return NULL;
	}
	new_list->head = NULL;
	new_list->size = 0;
	new_list->data_size = data_size;
    return new_list;
}

//function that adds a new node in the list of files
void dll_add_nth_node(dll_t* list, unsigned int n, const void* new_data)
{
	dll_node_t *node = malloc(sizeof(dll_node_t));
	node->data = malloc(list->data_size);
	node->next = NULL;
	node->prev = NULL;
	memcpy(node->data, new_data, list->data_size);
	if(list->size == 0 || n == 0) {
		node->next = NULL;
		list->head = node;
		list->size++;
	}
	else {
		dll_node_t *curr = list->head;
		unsigned int position = 0;
		while(position < n - 1 && position < list->size - 1) {
			curr = curr->next;
			position++;
		};
		node->next = curr->next;
		node->prev = curr;
		curr->next = node;
		list->size++;
	};
}

//function that removes a node from a doubly linked list
dll_node_t* dll_remove_nth_node(dll_t* list, unsigned int n)
{
	if (list == NULL || list->size == 0) {
		return NULL;
	};
	dll_node_t *removed;
	if (n == 0) {
		removed = list->head;
		list->head = removed->next;
		if (list->head != NULL) {
			list->head->prev = NULL;
		};
	} else {
		dll_node_t *curr = list->head;
		unsigned int position = 0;
		while (position < n - 1 && position < list->size - 1) {
			curr = curr->next;
			position++;
		};
		removed = curr->next;
		curr->next = removed->next;
		if (removed->next != NULL) {
			removed->next->prev = curr;
		};
	};
	list->size--;
	return removed;
}

//function that sorts a dll list of files by ids lexicographically
void sort_dll(dll_t* list) {
	dll_node_t *curr1 = list->head;
	for (unsigned int i = 0; i < list->size - 1; i++) {
		dll_node_t *curr2 = curr1->next;
		for (unsigned int j = i + 1; j < list->size; j++) {
			//get file from both nodes
			if (strcmp((*(file_t**)curr1->data)->id,
				(*(file_t**)curr2->data)->id) > 0) {
				void *aux = curr1->data;
				curr1->data = curr2->data;
				curr2->data = aux;
			};
			curr2 = curr2->next;
		};
		curr1 = curr1->next;
	};
}

//function that frees a doubly linked list
//it has a functionality parameter that indicates if the list stores
//files and they should also be freed
void dll_free(dll_t** pp_list, int files)
{
	dll_node_t *curr;
	curr = (*pp_list)->head;
	while(curr) {
		dll_node_t *next = curr->next;
		if (files) {
			free((*(file_t **)curr->data)->id);
			free(*(file_t **)curr->data);
		};
		free(curr->data);
		free(curr);
        curr = next;
	};
	free(*pp_list);
    *pp_list = NULL;
}