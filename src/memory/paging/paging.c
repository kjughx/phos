#include "memory/paging/paging.h"
#include "common.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
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

void paging_free_4gb(struct paging_4gb_chunck* chunk) {
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        kfree(table);
    }
    kfree(chunk->directory_entry);
    kfree(chunk);
}

void paging_switch(struct paging_4gb_chunck* directory) {
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
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

int paging_map(struct paging_4gb_chunck* directory, void* vaddr, void* paddr, int flags) {
    if (!PAGING_ALIGNED(vaddr) || !PAGING_ALIGNED(paddr))
        return -EINVAL;

    return paging_set(directory->directory_entry, vaddr, (pte_t)paddr | flags);
}

int paging_map_range(struct paging_4gb_chunck* directory, void* vaddr, void* paddr, uint32_t count,
                     int flags) {
    for (uint32_t i = 0; i < count; i++) {
        if (paging_map(directory, vaddr, paddr, flags) < 0)
            break;

        vaddr += PAGING_PAGE_SIZE;
        paddr += PAGING_PAGE_SIZE;
    }

    return 0;
}

int paging_map_to(struct paging_4gb_chunck* directory, void* vaddr, void* paddr, void* pend,
                  int flags) {
    if (!PAGING_ALIGNED(vaddr))
        return -EINVAL;
    if (!PAGING_ALIGNED(paddr))
        return -EINVAL;
    if (!PAGING_ALIGNED(pend))
        return -EINVAL;
    if ((pte_t)pend < (pte_t)paddr)
        return -EINVAL;

    uint32_t total_bytes = pend - paddr;
    uint32_t total_pages = total_bytes / PAGING_PAGE_SIZE;

    return paging_map_range(directory, vaddr, paddr, total_pages, flags);
}

int paging_get(pte_t* directory, void* vaddr, pte_t* entry) {
    int ret = 0;
    pte_t directory_index = 0;
    pte_t table_index = 0;
    if ((ret = paging_get_indexes(vaddr, &directory_index, &table_index)) < 0)
        return ret;

    pte_t* table = (pte_t*)(directory[directory_index] & 0xfffff000);
    *entry = table[table_index];

    return 0;
}
