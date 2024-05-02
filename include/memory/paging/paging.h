#ifndef _PAGING_H_
#define _PAGING_H_

#include "common.h"

#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

#define PAGING_ALIGNED(x) (((uint32_t)(x) % PAGING_PAGE_SIZE) == 0)

/* Page table entry */
typedef uint32_t pte_t;

struct paging_4gb_chunck {
    pte_t* directory_entry;
};

struct paging_4gb_chunck* paging_new_4gb(uint8_t flags);
pte_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunck* chunk);
void paging_switch(pte_t* directory);
extern void enable_paging();
int paging_set(pte_t* directory, void* vaddr, pte_t val);

#endif /* _PAGING_H_ */
