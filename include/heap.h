#ifndef HEAP_H
#define HEAP_H
//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
//define heap to organize files by priority
typedef struct heap_t
{
	void *arr;
	int size;
	int capacity;
	size_t data_size;
	int (*cmp)(void*, void*);
} heap_t;

//Index definitions for going through the heap

// Get the index of the parent
#define GO_UP(x) (((x) - 1) >> 1)

// Get the index of the left child
#define GO_LEFT(x) (((x) << 1) + 1)

// Get the index of the right child
#define GO_RIGHT(x) (((x) << 1) + 2)

//FUNCTIONS THAT HANDLE A HEAP

heap_t *heap_create(int (*cmp_f)(void*, void*), size_t data_size, int cap);
void heap_insert(heap_t *heap, void *data);
void *heap_top(heap_t *heap);
void heap_pop(heap_t *heap);
void heap_free(heap_t *heap);
int cmp(void *file_1, void *file_2);
#endif