#include "loader/formats/elfloader.h"
#include "common.h"
#include "config.h"
#include "fs/file.h"
#include "kernel.h"
#include "loader/formats/elf.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"

const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

static bool elf_valid_signature(void* buf) {
    return memcmp(buf, (void*)elf_signature, sizeof(elf_signature)) == 0;
}

static bool elf_valid_class(struct elf_header* header) {
    /* Only support 32 bit binaries */
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static bool elf_valid_encoding(struct elf_header* header) {
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static bool elf_is_executable(struct elf_header* header) {
    return header->e_type == ET_EXEC && header->e_entry >= PHOS_PROGRAM_VIRTUAL_ADDRESS;
}

static bool elf_has_program_header(struct elf_header* header) { return header->e_phoff != 0; }

void* elf_memory(struct elf_file* file) { return file->elf_memory; }

struct elf_header* elf_header(struct elf_file* file) { return file->elf_memory; }

struct elf32_shdr* elf_sheader(struct elf_header* header) {
    return (struct elf32_shdr*)((uint32_t)header + header->e_shoff);
}

struct elf32_phdr* elf_pheader(struct elf_header* header) {
    if (!header->e_phoff)
        return NULL;

    return (struct elf32_phdr*)((uint32_t)header + header->e_phoff);
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int index) {
    return &elf_pheader(header)[index];
}

struct elf32_shdr* elf_section(struct elf_header* header, int index) {
    return &elf_sheader(header)[index];
}

char* elf32_str_table(struct elf_header* header) {
    return (char*)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void* elf_virtual_base(struct elf_file* file) { return file->virtual_base_address; }

void* elf_virtual_end(struct elf_file* file) { return file->virtual_end_address; }

void* elf_physical_base(struct elf_file* file) { return file->physical_base_address; }

void* elf_physical_end(struct elf_file* file) { return file->physical_end_address; }

int elf_validate_load(struct elf_header* header) {
    if (!(elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) &&
          elf_has_program_header(header)))
        return -EINVAL;

    return 0;
}
