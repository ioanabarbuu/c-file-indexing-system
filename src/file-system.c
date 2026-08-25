//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file-system.h"
#include "heap.h"
#include "trie.h"

//FUNCTIONS THAT HANDLE THE FILE SYSTEM

//function that adds a file to the list of files
//associated to a word in the trie
void link_file_to_word(trie_t **trie, char *word, file_t *new_file)
{
	//use trie search to find already associated list
	dll_t **linked_files = (dll_t**)(trie_search(*trie, word));

	//check if the list exists
	dll_t *new_list;
	int is_word_old = 0;
	if(!linked_files) {
		new_list = dll_create(sizeof(file_t*));
	} else {
		is_word_old = 1;
		new_list = *linked_files; 
	};

	//check if file exists
	dll_node_t *curr = new_list->head;
	unsigned int i;
	for (i = 0; i < new_list->size; i++) {
		if (*(file_t **)curr->data == new_file) {
			break;
		}
		curr = curr->next;
	};
	if (i != new_list->size) {
		return;
	};

	//add file in file list associated with word
	dll_add_nth_node(new_list, new_list->size, &new_file);
	if (!is_word_old) {
		trie_insert(*trie, word, &new_list);
	};
}

//function that removes a file to the list of files
//associated to a word in the trie
void unlink_file_to_word(trie_t **trie, char *word,
						file_t *file_searched)
{
	//go through linked files of word
	dll_t **linked_files = (dll_t**)trie_search(*trie, word);
	if (!linked_files) {
		return;
	};
	dll_node_t *node = (*linked_files)->head;
	unsigned int j;
	for (j = 0; j < (*linked_files)->size; j++) {
		if (*(file_t**)node->data == file_searched) {
			dll_node_t *removed = dll_remove_nth_node((*linked_files), j);
			free(removed->data);
			free(removed);
			break;
		};
		node = node->next;
	};
	//if list is empty node should be removed
	if ((*linked_files)->size == 0) {
		trie_remove(*trie, word);
	};
	//check if file still has words linked to it!

}

//function that interprets the add file command
void add_file(char *command, dll_t **file_list, trie_t **trie, FILE *out)
{
	file_t *new_file = malloc(sizeof(file_t));

	//get file id
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	new_file->id = malloc(strlen(word) + 1);
	strcpy(new_file->id, word);

	//check if file exists
	dll_node_t *curr = (*file_list)->head;
	int found = 0;
	for (unsigned int i = 0; i < (*file_list)->size; i++) {
		if (strcmp((*(file_t **)curr->data)->id, new_file->id) == 0) {
			found = 1;
			break;
		};
		curr = curr->next;
	};

	if (found) {
		fprintf(out, "EXISTS\n");
		free(new_file->id);
		free(new_file);
		return;
	};
	fprintf(out, "OK\n");
	
	//get file priority and number of words 
	word = strtok(NULL, " \n\r");
	new_file->priority = atoi(word);
	word = strtok(NULL, " \n\r");
	int t = atoi(word);

	//get key words
	for (int i = 0; i < t; i++) {
		word = strtok(NULL, " \n\r");
		link_file_to_word(trie, word, new_file);
	};

	//add node in linked list of files
	dll_add_nth_node(*file_list, (*file_list)->size, &new_file);
}

//function that identifies the node in the dll of files by id
dll_node_t *find_file_by_id(dll_t **file_list, char *word,
							int *position, FILE *out)
{
	//search for file associated with id
	dll_node_t *curr = (*file_list)->head;
	*position = 0;
	for (unsigned int i = 0; i < (*file_list)->size; i++) {
		if (strcmp((*(file_t**)curr->data)->id, word) == 0) {
			break;
		} else {
			curr = curr->next;
			(*position)++;
		};
	};
	//check if files exist
	if(!curr) {
		fprintf(out, "NOT FOUND\n");
		return NULL;
	};
	return curr;
}

//function that interprets the delete file command
void delete_file(char *command, dll_t **file_list, trie_t **trie, FILE *out)
{
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	int position = 0;
	dll_node_t *curr = find_file_by_id(file_list, word, &position, out);
	if (!curr) {
		return;
	};
	go_through_trie(*trie, out, 1, curr, file_list);
	dll_node_t *removed = dll_remove_nth_node(*file_list, position);
	free((*(file_t **)removed->data)->id);
	free(*(file_t **)removed->data);
	free(removed->data);
	free(removed);
	fprintf(out, "OK\n");
}

//function that interprets the add key word
void add_key_word(char *command, dll_t **file_list, trie_t **trie, FILE *out)
{
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	int position = 0;
	dll_node_t *curr = find_file_by_id(file_list, word, &position, out);
	if (!curr) {
		return;
	};
	word = strtok(NULL, " \n\r");
	link_file_to_word(trie, word, *(file_t**)curr->data);
	fprintf(out, "OK\n");
}

//function that interprets the delete key word
void delete_key_word(char *command, dll_t **file_list,
					trie_t **trie, FILE *out)
{
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	int position = 0;
	dll_node_t *curr = find_file_by_id(file_list, word, &position, out);
	if (!curr) {
		return;
	};
	word = strtok(NULL, " \n\r");
	unlink_file_to_word(trie, word, *(file_t**)curr->data);
	//check if file should be deleted
	go_through_trie(*trie, out, 2, curr, file_list);
	fprintf(out, "OK\n");
}

//function that implements the find command
void find(char *command, trie_t **trie, FILE *out)
{
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");

	//get list of linked files
	dll_t **linked_files = (dll_t**)(trie_search(*trie, word));
	if (!linked_files || !(*linked_files)->size) {
		fprintf(out, "EMPTY\n");
		return;
	};
	fprintf(out, "%d ", (*linked_files)->size);
	sort_dll(*linked_files);

	dll_node_t *curr = (*linked_files)->head;
	for (unsigned int i = 0; i < (*linked_files)->size; i++) {
		fprintf(out, "%s ", (*(file_t**)curr->data)->id);
		curr = curr->next;
	};
	fprintf(out, "\n");
}

//function that implements the topk command
void topk(char *command, trie_t **trie, FILE *out)
{
	//parse the command
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	char *word2 = strtok(NULL, " \n\r");
	int k = atoi(word2);

	//get list of linked files
	dll_t **linked_files = (dll_t**)(trie_search(*trie, word));
	if (!linked_files || !(*linked_files)->size) {
		fprintf(out, "EMPTY\n");
		return;
	};

	//create the max heap
	heap_t *topk_heap = heap_create(cmp, sizeof(file_t*),
									(*linked_files)->size);
	dll_node_t *curr = (*linked_files)->head;
	for (unsigned int i = 0; i < (*linked_files)->size; i++) {
		heap_insert(topk_heap, curr->data);
		curr = curr->next;
	};
	unsigned int min = k;
	if (min > (*linked_files)->size) {
		min = (*linked_files)->size;
	};

	fprintf(out, "%d ", min);
	//pop the heap
	for (unsigned int i = 0; i < min; i++) {
		file_t *file_popped = *(file_t**)heap_top(topk_heap);
		heap_pop(topk_heap);
		fprintf(out, "%s ", file_popped->id);
	};
	fprintf(out, "\n");
	heap_free(topk_heap);
}

//function that implements the print command
void print(dll_t **file_list, trie_t **trie, FILE *out)
{
	go_through_trie(*trie, out, 3, NULL, file_list);
}

void prefix(char *command, trie_t **trie, FILE *out)
{
	//parse the command
	char *word = strtok(command, " \n\r");
	word = strtok(NULL, " \n\r");
	char key[101];
	strcpy(key, word);

	//get the starting node in the trie(the one where the prefix ends)
	trie_node_t *starting_node = word_end_node(*trie, key);
	if (!starting_node) {
		fprintf(out, "EMPTY\n");
		return;
	};

	//add the files linked to the prefix itself
	dll_t *prefix_list = dll_create(sizeof(file_t*));
	if (starting_node->end_of_word == 1) {
		dll_t **linked_files = (dll_t**)starting_node->value;
		dll_node_t *node = (*linked_files)->head;
		for (unsigned int i = 0; i < (*linked_files)->size; i++) {
			dll_add_nth_node(prefix_list, prefix_list->size,
						 	&(*(file_t**)node->data));
			node = node->next;
		};
	};

	//add the files linked to words that contain prefix
	go_through_trie_helper(key, *trie, out, starting_node, 4,
							NULL, NULL, NULL, prefix_list);
	if (prefix_list->size == 0) {
		fprintf(out, "EMPTY\n");
		dll_free(&prefix_list, 0);
		return;
	};
	sort_dll(prefix_list);
	dll_node_t *node = prefix_list->head;
	dll_node_t *previous = prefix_list->head;

	//get size of file list without duplicates
	int first = 1;
	int file_count = 0;
	for (unsigned int i = 0; i < prefix_list->size; i++) {
		if (first || strcmp((*(file_t**)node->data)->id,
							(*(file_t**)previous->data)->id)) {
			file_count++;
		};
		previous = node;
		node = node->next;
		first = 0;
	};
	fprintf(out, "%d ", file_count);

	//print file info without duplicates
	first = 1;
	node = prefix_list->head;
	previous = prefix_list->head;
	for (unsigned int i = 0; i < prefix_list->size; i++) {
		if (first || strcmp((*(file_t**)node->data)->id,
							(*(file_t**)previous->data)->id)) {
			fprintf(out, "%s ", (*(file_t**)node->data)->id);
		};
		previous = node;
		node = node->next;
		first = 0;
	};
	fprintf(out, "\n");
	dll_free(&prefix_list, 0);
}

//function that interprets a command and redirects the program
//to the corresponding function
void command_interpretor(char *command, dll_t **file_list,
						trie_t **trie, FILE *out)
{
	char *command_copy = malloc(strlen(command) + 1);
	strcpy(command_copy, command);
	char *word = strtok(command, " \n\r");
	if (!strcmp(word, "ADD")) {
		add_file(command_copy, file_list, trie, out);
	} else if (!strcmp(word, "DEL")) {
		delete_file(command_copy, file_list, trie, out);
	} else if (!strcmp(word, "ADDKW")) {
		add_key_word(command_copy, file_list, trie, out);
	} else if (!strcmp(word, "DELKW")) {
		delete_key_word(command_copy, file_list, trie, out);
	} else if (!strcmp(word, "FIND")) {
		find(command_copy, trie, out);
	} else if (!strcmp(word,  "TOPK")) {
		topk(command_copy, trie, out);
	} else if (!strcmp(word, "PRINT")) {
		print(file_list, trie, out);
	} else if (!strcmp(word, "PREFIX")) {
		prefix(command_copy, trie, out);
	};
	free(command_copy);
}

//functions that reads the data
void read_data(dll_t **file_list, trie_t **trie, FILE *in, FILE *out)
{
	int comm_count;
	fscanf(in, "%d ", &comm_count);

	//read the commands dynamically
	for (int i = 1; i <= comm_count; i++) {
		char *command = malloc(2);
		if (!command) {
			return;
		};
		command[0] = '\0';
		char curr_char = 'a';
		//i introduced a dummy char,
		// read until an endline is read
		while(curr_char != '\n' && fscanf(in, "%c", &curr_char) == 1) {
			char *aux_command = realloc(command, strlen(command) + 2);
			if (!aux_command) {
				return;
			};
			command = aux_command;
			int comm_length = strlen(command);
			command[comm_length] = curr_char;
			command[comm_length + 1] = '\0';
		};
		command_interpretor(command, file_list, trie, out);
		free(command);
	};
}