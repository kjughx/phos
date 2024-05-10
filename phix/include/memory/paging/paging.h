#ifndef _PAGING_H_
#define _PAGING_H_

#include "common.h"

// clang-format off
#define PAGING_IS_PRESENT      (1 << 0)
#define PAGING_IS_WRITABLE     (1 << 1)
#define PAGING_ACCESS_FROM_ALL (1 << 2)
#define PAGING_WRITE_THROUGH   (1 << 3)
#define PAGING_CACHE_DISABLED  (1 << 4)
//clang-format on

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

#define PAGE_ALIGN(x) (ALIGN((uint32_t)x, PAGING_PAGE_SIZE))
#define PAGE_ALIGN_LOWER(x) (ALIGN_LOWER((uint32_t)x, PAGING_PAGE_SIZE))
#define PAGING_ALIGNED(x) (((uint32_t)(x) % PAGING_PAGE_SIZE) == 0)

/* Page table entry */
typedef uint32_t pte_t;

/* @brief A paging chunk of 4GB */
struct paging_chunk {
    pte_t* directory_entry;
};

/* @brief Create a new 4GB paging chunk
 *
 * @param flags: The page flags
*/
struct paging_chunk* paging_new_chunk(uint8_t flags);

/* @brief Switch page directory
 *
 * @param directory: The page directory to switch to
*/
void paging_switch(struct paging_chunk* directory);

int paging_set(pte_t* directory, void* vaddr, pte_t val);

/* @brief Free a paging paging chunk
 *
 * @param chunk: The chunk to free
*/
void paging_free_chunk(struct paging_chunk* chunk);

/* @brief Map a virtual address to a physical address
 *
 * @param directory: The directory to map for
 * @param vaddr:     The virtual address that's mapped
 * @param paddr:     The physical address to map to
 * @param flags:     The page flags
*/
int paging_map(struct paging_chunk* directory, void* vaddr, void* paddr, uint8_t flags);

/* @brief Map a range of virtual addresses to a range of physical addresses
 *
 * @param directory: The directory to map for
 * @param vaddr:     The start of the range of virtual addresses
 * @param paddr:     The start of the range of physical address to map to
 * @param count:     How many addresses to map
*/
int paging_map_range(struct paging_chunk* directory, void* vaddr, void* paddr, uint32_t count,
                     uint8_t flags);

int paging_map_to(struct paging_chunk* directory, void* vaddr, void* paddr, void* pend,
                  uint8_t flags);

/* @brief Get the page table entry corresponding to a virtual address
 *
 * @param directory: The directory describing the mapping
 * @param vaddr:     The virtual address
 * param[out] entry: The corresponding entry
 *
 * @returns 0 if found
*/
int paging_get(pte_t* directory, void* vaddr, pte_t* entry);

extern void enable_paging();

void* paging_get_paddr(pte_t* directory, void* vaddr);
int paging_get_flags(pte_t* directory, void*vaddr);

struct interrupt_frame;
void page_fault_intr_cb(struct interrupt_frame* frame);

#endif /* _PAGING_H_ */
