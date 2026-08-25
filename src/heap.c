//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "file-system.h"
//FUNCTIONS THAT HANDLE A HEAP

//function that creates a heap (from lab skel)
heap_t *heap_create(int (*cmp_f)(void*, void*), size_t data_size, int cap)
{
	heap_t *heap;
	heap = malloc(sizeof(*heap));
	if (!heap) {
		return NULL;
	};
	heap_t heap_struct = (heap_t){.arr = malloc(cap * data_size),
								  .capacity = cap,
								  .data_size = data_size,
								  .size = 0,
								  .cmp = cmp_f};
	if(!heap_struct.arr) {
		free(heap);
		return NULL;
	};
	*heap = heap_struct;
	return heap;
}

static void heap_insert_fix(heap_t *heap, int pos)
{
	char *arr = heap->arr;
	char *aux = malloc(heap->data_size);
	if (!aux) {
		return;
	};
	int p = GO_UP(pos);
	int parent = p * heap->data_size;
	int curr = pos * heap->data_size;
	while (pos > 0 && heap->cmp(&arr[parent], &arr[curr]) > 0) {
		parent = p * heap->data_size;
		curr = pos * heap->data_size;
		memcpy(aux, &arr[parent], heap->data_size);
		memcpy(&arr[parent], &arr[curr], heap->data_size);
		memcpy(&arr[curr], aux, heap->data_size);
		pos = p;
		p = GO_UP(pos);
		parent = p * heap->data_size;
		curr = pos * heap->data_size;
	};
	free(aux);
}

//function that inserts an element in a heap
void heap_insert(heap_t *heap, void *data)
{
	char *arr = heap->arr;
	memcpy(&arr[heap->size * heap->data_size], data, heap->data_size);
	heap_insert_fix(heap, heap->size);
	heap->size++;
	if (heap->size == heap->capacity) {
		heap->capacity *= 2;

		heap->arr = realloc(heap->arr, heap->capacity * heap->data_size);
		if (!heap->arr) {
			free(heap);
			return;
		};
	};
}

//function that returns the top of the heap
void *heap_top(heap_t *heap)
{
	if (!heap || heap->size <= 0) {
		return NULL;
	};
	return heap->arr;
}

//function that fixes the pop on the heap
static void heap_pop_fix(heap_t *heap, int pos)
{
	int p = pos;
	int l = GO_LEFT(pos);
	int r = GO_RIGHT(pos);
	char *arr = heap->arr;
	char *aux = malloc(heap->data_size);
	if (!aux) {
		return;
	};
	while (l < heap->size) {
		int arr_pos = pos * heap->data_size;
		int arr_r = r * heap->data_size;
		int arr_l = l * heap->data_size;
		p = l;
		if (r < heap->size && heap->cmp(&arr[arr_r], &arr[arr_l]) < 0) {
			p = r;
		};
		int arr_p = p * heap->data_size;
		if (heap->cmp(&arr[arr_pos], &arr[arr_p]) <= 0) {
			break;
		};
		memcpy(aux, &arr[arr_pos], heap->data_size);
		memcpy(&arr[arr_pos],
			   &arr[arr_p], heap->data_size);
		memcpy(&arr[arr_p], aux, heap->data_size);
		pos = p;
		l = GO_LEFT(pos);
		r = GO_RIGHT(pos);
	};
	free(aux);
}

//function that pops a heap
void heap_pop(heap_t *heap)
{
	char *arr = heap->arr;
	memcpy(&arr[0], &arr[(heap->size - 1) * heap->data_size], heap->data_size);
	heap->size--;
	heap_pop_fix(heap, 0);
}

//function that frees a heap
void heap_free(heap_t *heap)
{
	free(heap->arr);
	free(heap);
}

//compare function for heap (it needs to compare two files)
int cmp(void *file_1, void *file_2)
{
	file_t *file1 = *(file_t**)file_1;
	file_t *file2 = *(file_t**)file_2;
	if (file1->priority != file2->priority) {
		return file2->priority - file1->priority;
	};
	return strcmp(file1->id, file2->id);
};