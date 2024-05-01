#include <stdint.h>

#include "kernel.h"
#include "memory/heap/kheap.h"
#include "paging.h"
#include "status.h"

static pte_t* current_directory = 0;

extern void paging_load_directory(pte_t* directory);

struct paging_4gb_chunck* paging_new_4gb(uint8_t flags) {
    pte_t* directory;
    struct paging_4gb_chunck* chunk;
    int offset = 0;

    if (!(directory = kzalloc(sizeof(pte_t) * PAGING_TOTAL_ENTRIES_PER_TABLE)))
        panic("Could not allocate paging directory");

    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        pte_t* entry = kzalloc(sizeof(pte_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        if (!entry)
            panic("Failed to allocate page table entry");

        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++) {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }

        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (pte_t)entry | flags | PAGING_IS_WRITABLE;
    }

    if (!(chunk = kzalloc(sizeof(struct paging_4gb_chunck))))
        panic("Could not allocate paging chunk");

    chunk->directory_entry = directory;

    return chunk;
}

void paging_switch(pte_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;
}

pte_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunck* chunk) {
    if (!chunk)
        panic("Invalid argument paging_4gb_chunk_get_directory");

    return chunk->directory_entry;
}

int paging_get_indexes(void* vaddr, pte_t* directory_index, pte_t* table_index) {
    if (!PAGING_ALIGNED(vaddr))
        return -EINVAL;

    *directory_index = ((pte_t)vaddr / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index =
        ((pte_t)vaddr % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE)) / PAGING_PAGE_SIZE;

    return 0;
}

int paging_set(pte_t* directory, void* vaddr, pte_t val) {
    pte_t directory_index = 0;
    pte_t table_index = 0;
    int ret;
    pte_t entry;
    pte_t* table;

    if (!PAGING_ALIGNED(vaddr))
        return -EINVAL;

    if ((ret = paging_get_indexes(vaddr, &directory_index, &table_index) < 0))
        return ret;

    entry = directory[directory_index];
    table = (pte_t*)(entry & 0xfffff000); /* Top 20 bits is the table address */
    table[table_index] = val;

    return 0;
}
