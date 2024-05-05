#ifndef _HEAP_H
#define _HEAP_H
#include "common.h"
#include "config.h"

#define HEAP_TYPE(x) ((x) & 0x0f)

#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01

#define ALIGN_BLOCK(x) ALIGN(x, PHOS_HEAP_BLOCK_SIZE)

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

/* @brief Create a new heap
 *
 * @param[out]   The new heap
 * @param p:     The start of the heap
 * @param end:   The end of the heap
 * @param table: A description of the heap
*/
int heap_create(struct heap* heap, void* p, void* end, struct heap_table* table);

/* @brief Allocate memory on the heap
 *
 * @param heap: The heap to allocate on
 * @param size: The amount of memory to allocate
*/
void* heap_malloc(struct heap* heap, size_t size);

/* @brief Free a heap */
void heap_free(struct heap* heap, void* p);

#endif /* _HEAP_H_ */
