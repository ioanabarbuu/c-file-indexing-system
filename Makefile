CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude

TARGETS = search_index

build: $(TARGETS)

search_index: src/tema2.c src/dll.c src/trie.c src/heap.c src/file-system.c
	$(CC) $(CFLAGS) src/tema2.c src/dll.c src/trie.c src/heap.c src/file-system.c -o search_index

run: build
	./search_index

clean:
	rm -f $(TARGETS)

.PHONY: build run clean pack