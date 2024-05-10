#ifndef _ELFLOADER_H_
#define _ELFLOADER_H_

#include "config.h"
#include "loader/formats/elf.h"

struct elf_file {
    char filename[PHOS_MAX_PATH];
    int in_memory_size;
    void* elf_memory;           /* Physical address of elf file */
    void* virtual_base_address; /* Virtual base address of the binary */
    void* virtual_end_address;
    void* physical_base_address;
    void* physical_end_address;
};

struct elf_header* elf_header(struct elf_file* file);
void* elf_virtual_base(struct elf_file* file);
void* elf_virtual_end(struct elf_file* file);
void* elf_physical_base(struct elf_file* file);
void* elf_physical_end(struct elf_file* file);
struct elf32_phdr* elf_pheaders(struct elf_header* header);
struct elf32_phdr* elf_program_header(struct elf_header* header, int index);
void* elf_phdr_paddr(struct elf_file* elf_file, struct elf32_phdr* phdr);

int elf_load(const char* filename, struct elf_file** file);
void elf_close(struct elf_file* elf_file);

#endif /* _ELFLOADER_H_ */
