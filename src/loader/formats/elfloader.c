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
#include "task/task.h"

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

struct elf32_phdr* elf_pheaders(struct elf_header* header) {
    if (!header->e_phoff)
        return NULL;

    return (struct elf32_phdr*)((uint32_t)header + header->e_phoff);
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int index) {
    return &elf_pheaders(header)[index];
}

struct elf32_shdr* elf_section(struct elf_header* header, int index) {
    return &elf_sheader(header)[index];
}

void* elf_phdr_paddr(struct elf_file* elf_file, struct elf32_phdr* phdr) {
    return elf_file->elf_memory + phdr->p_offset;
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
        return -EBADFORMAT;

    return 0;
}

int elf_process_phdr_pt_load(struct elf_file* elf_file, struct elf32_phdr* phdr) {
    /* Set the lowest virtual address */
    if (!elf_virtual_base(elf_file) || elf_virtual_base(elf_file) >= (void*)phdr->p_vaddr) {
        elf_file->virtual_base_address = (void*) phdr->p_vaddr;
        elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
    }

    /* Set the highest virtual address */
    if (!elf_virtual_end(elf_file) || elf_virtual_end(elf_file) <= (void*)phdr->p_vaddr + phdr->p_filesz) {
        elf_file->virtual_end_address = (void*) phdr->p_vaddr + phdr->p_filesz;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }

    return 0;
}

int elf_process_pheader(struct elf_file* elf_file, struct elf32_phdr* phdr) {

    switch (phdr->p_type) {
        case PT_LOAD: {
            return elf_process_phdr_pt_load(elf_file, phdr);
        } break;
        default:
            return -EINVAL;
    }
}

int elf_process_pheaders(struct elf_file* elf_file) {
    struct elf_header* header;
    int ret = 0;

    header = elf_header(elf_file);

    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr* phdr = elf_program_header(header, i);
        if ((ret  = elf_process_pheader(elf_file, phdr)) < 0)
            return ret;
    }

    return 0;
}

int elf_process_loaded(struct elf_file* elf_file) {
    struct elf_header* header;
    int ret = 0;

    header = elf_header(elf_file);

    if ((ret = elf_validate_load(header)) < 0)
        return ret;

    if ((ret = elf_process_pheaders(elf_file)) < 0)
        return ret;

    return 0;
}

int elf_load(const char* filename, struct elf_file** file) {
    struct elf_file *elf_file;
    struct file_stat stat;
    int fd = 0;
    int ret = 0;

    if (!(elf_file = kzalloc(sizeof(struct elf_file))))
        return -ENOMEM;

    if ((ret = fopen(filename, "r")) < 0)
        goto out;

    fd = ret;
    if ((ret = fstat(fd, &stat)) < 0)
        goto out;

    if (!(elf_file->elf_memory = kzalloc(stat.filesize)))
        goto out;

    if((ret = fread(elf_file->elf_memory, stat.filesize, 1, fd)) < 0)
        goto out;

    if ((ret = elf_process_loaded(elf_file)) < 0)
        goto out;

    *file = elf_file;

out:
    if (ret) {
        kfree(elf_file->elf_memory);
        kfree(elf_file);
    }
    return ret;
}

void elf_close(struct elf_file* elf_file) {
    if (!elf_file)
        return;

    kfree(elf_file->elf_memory);
    kfree(elf_file);
}
