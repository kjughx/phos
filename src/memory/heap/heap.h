#ifndef _HEAP_H
#define _HEAP_H
#include "config.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01

#define HEAP_BLOCK_HAS_NEXT (1 << 8)
#define HEAP_BLOCK_IS_FIRST (1 << 7)

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
};

struct heap {
    struct heap_table* table;
    void* saddr; /* Start address of the heap data pool */
};

int heap_create(struct heap* heap, void* p, void* end, struct heap_table* table);
void* heap_malloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* p);

#endif /* _HEAP_H_ */
