//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"
#include "trie.h"
#include "file-system.h"

int main(void)
{
	FILE *in = fopen("indexare.in", "r");
	FILE *out = fopen("indexare.out", "wt");
	if(!in) {
		return 1;
	};
	dll_t *file_list = dll_create(sizeof(file_t*));
	trie_t *trie = trie_create(sizeof(dll_t*), 26, free_trie_data);
	read_data(&file_list, &trie, in, out);
	trie_free(&trie);
	dll_free(&file_list, 1);
	fclose(in);
	fclose(out);
	return 0;
}