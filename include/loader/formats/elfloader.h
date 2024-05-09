#ifndef _ELFLOADER_H_
#define _ELFLOADER_H_

#include "config.h"
#include "loader/formats/elf.h"

struct elf_file {
    char filename[PHOS_MAX_PATH];
    int in_memory_size;
    void* elf_memory; /* Physical address of elf file */
    void* virtual_base_address; /* Virtual base address of the binary */
    void* virtual_end_address;
    void* physical_base_address;
    void* physical_end_address;
};

#endif /* _ELFLOADER_H_ */
