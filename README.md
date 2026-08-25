# File Indexing System

This project implements a simplified file indexing system in C that manages
files and their associated key words. It uses custom data structures to
support efficient file insertion, deletion, exact search, prefix search
and priority-based retrieval.

## 1. Reading Commands

> To read commands, I opted for an implementation that reads from the input
file each character and builds the current command string via reallocations
each time a new character is read, until an endline is reached (the `read_data`
function). Then, the command is passed to the `command_interpretor` function
for parsing and execution. I chose this approach because no maximum command
length was specified in the problem text, and the ADD command can contain a
variable number of key words, so this was the only approach that doesn't
allocate unnecessary memory. Another problem I had here was that one test had
a \r character that ruined my strtok logic and lead to segmentation faults.
I fixed that by introducing a \r as a separation character as well.

> The function `command_interpretor` first makes a copy of the command, because
`strtok` modifies the string it parses. The original command is used only to
detect the command and find in which function the program should be redirected,
while the copy is sent to the function, where the actual arguments are parsed
and processed.

## 2. Data Structures Used

> The file system is built with three main data structures: a doubly linked
list, a trie, and a heap, as the homework test suggested.

> The doubly linked list is generic, storing data as `void *` and it is used
in two places. First, it stores all the files in the system, each node
storing a pointer to a `file_t` struct. Second, for each word in the trie, the
value of that node consists of a linked list of pointers to the files
associated with that word.

> A challenge caused by this design was how to free the nodes of a dll_t
struct. The main file list stores the actual `file_t` structures and their ID,
while the lists inside the trie only store pointers to files. This means that
when freeing a trie list, only the nodes should be freed, not the files
themselves. To solve this, the `dll_free` function has a parameter that
specifies whether the actual files should also be freed. For trie lists, this
parameter is 0, and for the main file list it is 1.

> The trie is used to store all key words of the file system. Each end-of-word
node stores as value a pointer to the linked list of files associated with that
word. A challenging part of the implementation was that the value stored in a
trie node is not directly a `dll_t *`, but a pointer to that. Because of this,
when freeing a trie value, both the linked list and the pointer have to be
freed. This is handled by the `free_trie_data` function, which I thought was
the easiest approach.

> The heap is used for the TOPK command. For a given word, all associated files
are inserted into a heap ordered by priority and if priority doesn't work,
lexicographically. This allows the first k files to be extracted in the
required order. The heap stores file pointers, not actual file structures.

## 4. Functionality

> The ADD command creates a new `file_t`, checks if a file with the same ID
already exists, and then links the new file to all of its key words.
If the file already exists the command prints EXISTS. A problem I had here
was related to adding a file to a word that already existed in the trie.
Initially, I called `trie_insert` every time a file was linked to a word,
but this was problematic because my `trie_insert` function frees the old
value if a word already exists. This caused the list of files for that word to
be freed causing segmentation faults. I fixed this by calling `trie_insert`
only when the word is new; if the word already exists, I only add the pointer
to the existing list.

> The DEL command first finds the file by ID in the main file list. Then, the
program goes through the trie using `go_through_trie` with a functionality
parameter that indicates what should be done when going through the trie.
It removes pointers to that file from every list in the trie.
After all references are removed, the file is also removed from the main list

> The ADDKW command finds the file by ID and links it to the given word. The
same `link_file_to_word` function used by ADD is reused here. The function also
checks if the file is already associated with that word, so the same file is
not added twice in the word list.

> The DELKW command removes a file from the list associated with one key word.
After removing the pointer, the program goes through the trie to check if the
file is still associated with one other word. If not, the file is
removed completely from the main file list.

> The FIND command searches the trie for an exact word. If the word does not
exist or has no associated files, EMPTY is printed. Otherwise, the linked list
of files associated with the word is sorted lexicographically and then printed.
I used selection sort to sort a doubly linked list (`sort_dll`).The sorting is
done directly on the list by swapping node data.

> The TOPK command also starts by searching the trie for the given word. If the
word exists, all files associated with it are added into a heap. The compare
function orders files by higher priority first, and if two priorities are
equal,lexicographically. Then, the first k files are popped from the heap.

> The PREFIX command finds the trie node where the prefix ends, then goes
through all children of that node to collect files associated with every word
that has the given prefix. The files are placed into an additional linked list,
then sorted lexicographically. Since the same file can appear for multiple
words with the same prefix, duplicates are removed only when printing,
with the help of comparing consecutive IDS after sorting. A problem I had here
was that the recursive go through trie function modifies the key string while
building words. Passing the pointer returned by strtok directly was problematic
when running valgrind. I fixed this by copying the prefix into a separate
`char key[101]` before passing it to the `go_through_trie_helper` function.

> The PRINT command goes through the whole trie in lexicographical order and
prints each word that still has at least one associated file. Before printing
the files for a word, their list is sorted. As it can be noticed by now, the
same go_through function is reused for multiple operations via a functionality
parameter, which made the implementation more compact,  even though it made
the helper function quite complex.

## Concepts Used

- dynamic memory allocation
- pointers and `void *`
- doubly linked lists
- tries
- heaps
- priority-based ordering
- lexicographical sorting
- selection sort
- generic data structures
- string parsing with `strtok`
- command parsing
- prefix searching
- recursive trie traversal
- file indexing
- memory management
- file input and output