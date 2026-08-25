//Barbu Ioana-Steliana, 312CA, Tema 2 - SDA
#ifndef DLL_H
#define DLL_H
typedef struct file_t file_t;

//define node of a doubly linked list that stores files
typedef struct dll_node_t
{
    void* data;
    struct dll_node_t *prev, *next;
} dll_node_t;

//define doubly linked list that stores files
typedef struct dll_t
{
    dll_node_t *head;
	unsigned int data_size;
    unsigned int size;
} dll_t;

//FUNCTIONS THAT HANDLE A DOUBLY LINKED LIST

dll_t *dll_create(unsigned int data_size);
void dll_add_nth_node(dll_t* list, unsigned int n, const void* new_data);
dll_node_t* dll_remove_nth_node(dll_t* list, unsigned int n);
void sort_dll(dll_t* list);
void dll_free(dll_t** pp_list, int files);
#endif