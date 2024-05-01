#include "kheap.h"
#include "config.h"
#include "heap.h"
#include "memory/memory.h"
#include "string/string.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init() {
    int total_table_entries = PHOS_HEAP_SIZE_BYTES / PHOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)PHOS_HEAP_TABLE_ADDRESS;
    kernel_heap_table.total = total_table_entries;

    void* end = (void*)(PHOS_HEAP_SADDRESS + PHOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)PHOS_HEAP_SADDRESS, end, &kernel_heap_table);
    if (res < 0) {
        print("Failed to create heap\n");
    }
}

void* kmalloc(size_t size) { return heap_malloc(&kernel_heap, size); }
void* kzalloc(size_t size) {
    void* p = heap_malloc(&kernel_heap, size);
    memset(p, 0, size);
    return p;
}

void kfree(void* p) { return heap_free(&kernel_heap, p); }
