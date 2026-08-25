//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include "dll.h"
#include "trie.h"
//define file type
typedef struct file_t
{
    char *id;
    int priority;
} file_t;

//FUNCTIONS THAT HANDLE THE FILE SYSTEM

void link_file_to_word(trie_t **trie, char *word, file_t *new_file);
void unlink_file_to_word(trie_t **trie, char *word, file_t *file_searched);
void add_file(char *command, dll_t **file_list, trie_t **trie, FILE *out);
dll_node_t *find_file_by_id(dll_t **file_list, char *word,
                            int *position, FILE *out);
void delete_file(char *command, dll_t **file_list, trie_t **trie, FILE *out);
void add_key_word(char *command, dll_t **file_list, trie_t **trie, FILE *out);
void delete_key_word(char *command, dll_t **file_list,
					trie_t **trie, FILE *out);
void find(char *command, trie_t **trie, FILE *out);
void topk(char *command, trie_t **trie, FILE *out);
void print(dll_t **file_list, trie_t **trie, FILE *out);
void prefix(char *command, trie_t **trie, FILE *out);
void command_interpretor(char *command, dll_t **file_list,
						trie_t **trie, FILE *out);
void read_data(dll_t **file_list, trie_t **trie, FILE *in, FILE *out);
#endif