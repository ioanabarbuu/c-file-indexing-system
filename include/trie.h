#ifndef TRIE_H
#define TRIE_H
//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include "dll.h"
//define trie node
typedef struct trie_node_t
{
    void* value;
    int end_of_word;
	struct trie_node_t** children;
    int n_children;
} trie_node_t;

//define trie to store key words of files
typedef struct trie_t
{
    struct trie_node_t* root;
    int size;
    int data_size;
    int alphabet_size;
    void (*free_value_cb)(void*);
    int nNodes;
} trie_t;

//FUNCTIONS THAT HANDLE TRIE

trie_node_t* trie_create_node(trie_t * trie);
trie_t* trie_create(int data_size, int alphabet_size,
                    void (*free_value_cb)(void*));
void trie_insert_helper(trie_t* trie, trie_node_t* node,
						char* key, void* value);
void trie_insert(trie_t* trie, char* key, void* value);
void* trie_search_helper(trie_t* trie, trie_node_t* node, char* key);
void* trie_search(trie_t* trie, char* key);
trie_node_t* word_end_node_helper(trie_t* trie, trie_node_t* node, char* key);
trie_node_t* word_end_node(trie_t* trie, char* key);
int trie_remove_helper(trie_t* trie, trie_node_t* node, char* key);
void trie_remove(trie_t* trie, char* key);
void trie_free_helper(trie_t* trie, trie_node_t* node);
void trie_free(trie_t** pTrie);
void free_trie_data(void *node);
void go_through_trie_helper(char *key, trie_t *trie, FILE *out,
							trie_node_t *curr, int functionality,
							dll_node_t *file_searched, int *is_ok,
							int *empty, dll_t *prefix_list);
void go_through_trie(trie_t *trie, FILE *out, int functionality,
					dll_node_t *file_searched, dll_t **file_list);
#endif