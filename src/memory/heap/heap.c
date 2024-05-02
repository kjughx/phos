#include "heap.h"
#include "common.h"
#include "config.h"
#include "memory/memory.h"
#include "status.h"
#include "string/string.h"

#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a)-1)
#define ALIGN_BLOCK(x) ALIGN(x, PHOS_HEAP_BLOCK_SIZE)

#define HEAP_TYPE(x) ((x) & 0x0f)

static int heap_validate_table(void* p, void* end, struct heap_table* table) {
    size_t table_size = (size_t)(end - p);
    size_t total_blocks = table_size / PHOS_HEAP_BLOCK_SIZE;

    if (table->total != total_blocks)
        return -EINVAL;

    return 0;
}

static inline bool heap_validate_alignment(void* p) {
    return ((unsigned int)p % PHOS_HEAP_BLOCK_SIZE) == 0;
}

static int heap_get_start_block(struct heap* heap, uint32_t total_blocks) {
    struct heap_table* table = heap->table;
    uint32_t bc = 0;
    int bs = -1;
    for (size_t i = 0; i < table->total; i++) {
        if (HEAP_TYPE(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }

        if (bs == -1)
            bs = i;
        bc++;
        if (bc == total_blocks)
            break;
    }

    if (bs == -1)
        return -ENOMEM;

    return bs;
}

static inline void* heap_block_to_address(struct heap* heap, uint32_t block) {
    return heap->saddr + (block * PHOS_HEAP_BLOCK_SIZE);
}

static inline int heap_address_to_block(struct heap* heap, void* p) {
    return ((int)(p - heap->saddr)) / PHOS_HEAP_BLOCK_SIZE;
}

static void heap_mark_blocks_taken(struct heap* heap, uint32_t start, uint32_t total_blocks) {
    int end_block = (start + total_blocks) - 1;
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1)
        entry |= HEAP_BLOCK_HAS_NEXT;

    for (int i = start; i <= end_block; i++) {
        heap->table->entries[i] = entry;

        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1)
            entry |= HEAP_BLOCK_HAS_NEXT;
    }
}

static void heap_mark_blocks_free(struct heap* heap, uint32_t start) {
    struct heap_table* table = heap->table;

    for (size_t i = start; i < table->total; i++) {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        if (!(entry & HEAP_BLOCK_HAS_NEXT))
            break;
    }
}

int heap_create(struct heap* heap, void* p, void* end, struct heap_table* table) {
    int ret = 0;

    if (!heap_validate_alignment(p) || !heap_validate_alignment(end))
        return -EINVAL;

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = p;
    heap->table = table;

    ret = heap_validate_table(p, end, table);
    if (ret)
        return ret;

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return 0;
}

void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks) {
    void* address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0)
        return NULL;

    address = heap_block_to_address(heap, start_block);
    heap_mark_blocks_taken(heap, start_block, total_blocks);

    return address;
}

void* heap_malloc(struct heap* heap, size_t size) {
    int aligned = ALIGN_BLOCK(size);
    return heap_malloc_blocks(heap, aligned / PHOS_HEAP_BLOCK_SIZE);
}

void heap_free(struct heap* heap, void* p) {
    if (!p)
        return;

    heap_mark_blocks_free(heap, heap_address_to_block(heap, p));
}
