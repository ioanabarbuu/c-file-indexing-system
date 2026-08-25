//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include "dll.h"
#include "file-system.h"


//FUNCTIONS THAT HANDLE TRIE

//function that creates a node in a trie
trie_node_t* trie_create_node(trie_t * trie)
{
    trie_node_t* new_node = malloc(sizeof(*new_node));
    if (!new_node) {
        return NULL;
    };
    new_node->value = NULL;
    new_node->end_of_word = 0;
    new_node->n_children = 0;
    trie->nNodes++;
    new_node->children = calloc(trie->alphabet_size, sizeof(trie_node_t*));
    if (!new_node->children) {
        free(new_node);
        return NULL;
    };
    return new_node;
}

//function that creates a trie
trie_t* trie_create(int data_size, int alphabet_size,
					void (*free_value_cb)(void*))
{
    trie_t* trie = malloc(sizeof(*trie));
    if (!trie) {
        return NULL;
    };
    trie->free_value_cb = free_value_cb;
    trie->alphabet_size = alphabet_size;
    trie->size = 0;
    trie->data_size = data_size;
    trie->nNodes = 0;
    trie->root = trie_create_node(trie);
    if (!trie->root) {
        free(trie);
        return NULL;
    };
    return trie;
}

//functions that insert a word in a trie
void trie_insert_helper(trie_t* trie, trie_node_t* node,
						char* key, void* value)
{
    if (strlen(key) == 0) {
        if (node->end_of_word) {
            trie->free_value_cb(node->value);
        } else {
            trie->size++;
        };
        node->value = malloc(trie->data_size);
        memcpy(node->value, value, trie->data_size);
        node->end_of_word = 1;
        return;
    };
    int index = key[0] - 'a';
    if (index < 0 || index >= trie->alphabet_size) {
        return;
    };
    trie_node_t* next_node = node->children[index];
    if (next_node == NULL) {
        next_node = trie_create_node(trie);
        node->children[index] = next_node;
        node->n_children++;
    }
    trie_insert_helper(trie, next_node, key + 1, value);
}

void trie_insert(trie_t* trie, char* key, void* value)
{
    trie_insert_helper(trie, trie->root, key, value);
}

//function that searches an element in a trie
void* trie_search_helper(trie_t* trie, trie_node_t* node, char* key)
{
    if (key[0] == '\0') {
        return node->value;
    };
    int index = key[0] - 'a';
    trie_node_t* next_node = node->children[index];
    if (!next_node) {
        return NULL;
    };
    return trie_search_helper(trie, next_node, key + 1);
}

void* trie_search(trie_t* trie, char* key)
{
    return trie_search_helper(trie, trie->root, key);
}

trie_node_t* word_end_node_helper(trie_t* trie, trie_node_t* node, char* key)
{
	if (key[0] == '\0') {
        return node;
    };
    int index = key[0] - 'a';
    trie_node_t* next_node = node->children[index];
    if (!next_node) {
        return NULL;
    };
    return word_end_node_helper(trie, next_node, key + 1);
};

trie_node_t* word_end_node(trie_t* trie, char* key)
{
	return word_end_node_helper(trie, trie->root, key);
};

//functions that remove an element from a trie
int trie_remove_helper(trie_t* trie, trie_node_t* node, char* key)
{
    if (!strlen(key)) {
        if (node->end_of_word == 1) {
            trie->free_value_cb(node->value);
            node->value = NULL;
            node->end_of_word = 0;
            trie->size--;
            if (node->n_children == 0) {
                return 1;
            } else {
            return 0;
            };
        };
        return 0;
    };
    int index = key[0] - 'a';
    trie_node_t* next_node = node->children[index];
    if (next_node && trie_remove_helper(trie, next_node, key + 1) == 1) {
        free(next_node->children);
        free(next_node);
        node->children[index] = NULL;
        node->n_children--;
        trie->nNodes--;
        if (node->n_children == 0 && node->end_of_word == 0) {
            return 1;
        };
    };
    return 0;
}

void trie_remove(trie_t* trie, char* key)
{
    trie_remove_helper(trie, trie->root, key);
}

//functions that free a trie
void trie_free_helper(trie_t* trie, trie_node_t* node)
{
    if (node == NULL) {
        return;
    };
    for (int i = 0; i < trie->alphabet_size; i++) {
        if (node->children[i] != NULL) {
            trie_free_helper(trie, node->children[i]);
        };
    };
    if (node->value != NULL) {
        trie->free_value_cb(node->value);
    };
    free(node->children);
    free(node);
}

void trie_free(trie_t** pTrie)
{
    trie_t *trie = *pTrie;
    trie_free_helper(trie, trie->root);
    free(trie);
    *pTrie = NULL;
}

//function that frees the data associated to a word in the trie
void free_trie_data(void *node)
{
	dll_t *list = *(dll_t **)node;
	dll_free(&list, 0);
	free(node);
}

//functions that go through a trie lexicographically
//the functionality parameter is used to separate the
//things this function can be used for
void go_through_trie_helper(char *key, trie_t *trie, FILE *out,
							trie_node_t *curr, int functionality,
							dll_node_t *file_searched, int *is_ok,
							int *empty, dll_t *prefix_list)
{
	for (int i = 0; i < trie->alphabet_size; i++) {
		if (curr->children[i]!= NULL) {
			int length = strlen(key);
			key[length] = i + 'a';
			key[length + 1] = '\0';
			//check if it is a word
			if (curr->children[i]->end_of_word == 1) {
				//if functionality is 1 we are looking for a 
				//file to delete the reference
				if (functionality == 1 && file_searched) {
					//check if the word has the searched file associated
					dll_t **linked_files = ((dll_t**)curr->children[i]->value);
					dll_node_t *node = (*linked_files)->head;
					for (unsigned int j = 0; j < (*linked_files)->size; j++) {
						if (*(file_t **)node->data ==
							*(file_t **)file_searched->data) {
							dll_node_t *removed =
							dll_remove_nth_node((*linked_files), j);
							free(removed->data);
							free(removed);
							break;
						};
						node = node->next;
					};
				};
				//if functionality is 2, we are checking if a file has any
				//more words associated after a remove word opperation
				if (functionality == 2 && !(*is_ok) && file_searched) {
					//check if the word has the searched file associated
					dll_t **linked_files = ((dll_t**)curr->children[i]->value);
					dll_node_t *node = (*linked_files)->head;
					for (unsigned int j = 0; j < (*linked_files)->size; j++) {
						if (*(file_t **)node->data ==
							*(file_t **)file_searched->data) {
							*is_ok = 1;
							break;
						};
						node = node->next;
					};
				};
				//if functionality is 3, we are implementing the print
				if (functionality == 3) {
					dll_t **linked_files = ((dll_t**)curr->children[i]->value);
					if ((*linked_files)->size) {
						fprintf(out, "%s ", key);
						fprintf(out, "%d ", (*linked_files)->size);
						dll_node_t *node = (*linked_files)->head;
						sort_dll(*linked_files);
						for (unsigned int i = 0; i <
							(*linked_files)->size; i++) {
							fprintf(out, "%s ", (*(file_t**)node->data)->id);
							node = node->next;
						};
						fprintf(out, "\n");
						(*empty) = 1;
					};
				};
				//for func 4 make list of files associated with prefix
				if (functionality == 4) {
					dll_t **linked_files = ((dll_t**)curr->children[i]->value);
					dll_node_t *node = (*linked_files)->head;
					for (unsigned int i = 0; i < (*linked_files)->size; i++) {
						dll_add_nth_node(prefix_list, prefix_list->size,
										&(*(file_t**)node->data));
						node = node->next;
					};
				};
			};
			go_through_trie_helper(key, trie, out, curr->children[i],
									functionality, file_searched, is_ok,
									empty, prefix_list);
			key[length] = '\0';
		};
	};
}

void go_through_trie(trie_t *trie, FILE *out, int functionality,
					dll_node_t *file_searched, dll_t **file_list)
{
	int is_ok = 0;
	int empty = 0;
	char key[101] = "";
	go_through_trie_helper(key, trie, out, trie->root, functionality,
							file_searched, &is_ok, &empty, NULL);

	//search for the file and remove it
	if( functionality == 2 && !is_ok) {
		int position = 0;
		dll_node_t *curr = (*file_list)->head;
		for (unsigned int i = 0; i < (*file_list)->size; i++) {
			if (*(file_t **)curr->data ==
				*(file_t **)file_searched->data) {
				break;
			};
			curr = curr->next;
			position++;
		};
		dll_node_t *removed = dll_remove_nth_node(*file_list, position);
		free((*(file_t **)removed->data)->id);
		free(*(file_t **)removed->data);
		free(removed->data);
		free(removed);
	};
	if (functionality == 3 && !empty) {
        fprintf(out, "EMPTY\n");
    };
}
