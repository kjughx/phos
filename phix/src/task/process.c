#include "task/process.h"
#include "common.h"
#include "config.h"
#include "fs/file.h"
#include "kernel.h"
#include "loader/formats/elf.h"
#include "loader/formats/elfloader.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"
#include "task/task.h"

/* Currently running process */
struct process* current_process = NULL;

/* All running processes */
static struct process* processes[PHIX_MAX_PROCESSES] = {0};

static void process_init(struct process* process) { memset(process, 0, sizeof(struct process)); }

struct process* process_current() { return current_process; }

struct process* process_get(int pid) {
    if (pid < 0 || pid >= PHIX_MAX_PROCESSES)
        return ERROR(-EINVAL);

    return processes[pid];
}

static int process_load_elf(const char* filename, struct process* process) {
    struct elf_file* elf_file;
    int ret = 0;

    if ((ret = elf_load(filename, &elf_file)) < 0)
        return ret;

    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;

    return 0;
}

static int process_load_binary(const char* filename, struct process* process) {
    int fd = -1;
    int ret = 0;
    struct file_stat stat;
    void* program_data = NULL;

    if ((fd = fopen(filename, "r")) < 0)
        return fd;

    if ((ret = fstat(fd, &stat)) < 0)
        goto out;

    if (!(program_data = kzalloc(stat.filesize))) {
        ret = -ENOMEM;
        goto out;
    }

    if ((ret = fread(program_data, stat.filesize, 1, fd)) != 1)
        goto out;

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->p = program_data;
    process->size = stat.filesize;

out:
    if (ret < 0)
        kfree(program_data);

    fclose(fd);
    return ret;
}

static int process_load_data(const char* filename, struct process* process) {
    int ret = 0;
    ret = process_load_elf(filename, process);
    if (ret == -EBADFORMAT)
        return process_load_binary(filename, process);

    return ret;
}

static int process_map_elf(struct process* process) {
    int ret = 0;
    struct elf_file* elf_file = process->elf_file;
    struct elf_header* header = elf_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++) {
        struct elf32_phdr* phdr = elf_program_header(header, i);
        uint8_t flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W)
            flags |= PAGING_IS_WRITABLE;

        void* phdr_paddr = elf_phdr_paddr(elf_file, phdr);
        ret = paging_map_to(process->task->page_directory, (void*)PAGE_ALIGN_LOWER(phdr->p_vaddr),
                            (void*)PAGE_ALIGN_LOWER(phdr_paddr),
                            (void*)PAGE_ALIGN(phdr_paddr + phdr->p_memsz), flags);
        if (ret < 0)
            break;
    }

    return ret;
}

static int process_map_binary(struct process* process) {
    return paging_map_to(process->task->page_directory, (void*)PHIX_PROGRAM_VIRTUAL_ADDRESS,
                         process->p, (void*)PAGE_ALIGN(process->p + process->size),
                         PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
}

int process_map_memory(struct process* process) {
    int ret = 0;

    /* Map the program */
    switch (process->filetype) {
    case PROCESS_FILETYPE_ELF: {
        ret = process_map_elf(process);
    } break;
    case PROCESS_FILETYPE_BINARY: {
        ret = process_map_binary(process);
    } break;
    default:
        panic("process_map_memory: Unexpected filetype");
    }
    if (ret < 0)
        return ret;

    /* Map the stack */
    return paging_map_to(process->task->page_directory,
                         (void*)PHIX_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack,
                         (void*)PAGE_ALIGN(process->stack + PHIX_USER_PROGRAM_STACK_SIZE),
                         PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL);
}

static int process_load_for_slot(const char* filename, struct process** process, int process_slot) {
    int ret = 0;
    struct task* task = NULL;
    struct process* _process = NULL;
    void* program_sp = NULL;

    _process = process_get(process_slot);
    if (ISERR(_process))
        return PTR_ERR(_process);

    if (_process)
        return -EISTKN;

    if (!(_process = kzalloc(sizeof(struct process))))
        return -ENOMEM;

    process_init(_process);
    if ((ret = process_load_data(filename, _process)) < 0)
        goto out;

    if (!(program_sp = kzalloc(PHIX_USER_PROGRAM_STACK_SIZE))) {
        ret = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_sp;
    _process->id = process_slot;

    task = task_new(_process);
    if (ISERR(task)) {
        ret = PTR_ERR(task);
        goto out;
    }

    _process->task = task;

    if ((ret = process_map_memory(_process)) < 0)
        goto out;

    *process = _process;

    /* Add process to the array */
    processes[process_slot] = _process;

out:
    if (ret < 0) {
        // kfree(_process); TODO: Free the memory in _process
        kfree(program_sp);
        task_free(task);
    }

    return ret;
}

static int process_get_free_slot() {
    for (int i = 0; i < PHIX_MAX_PROCESSES; i++) {
        if (processes[i] == NULL)
            return i;
    }

    return -EISTKN;
}

int process_load(const char* filename, struct process** process) {
    int process_slot = process_get_free_slot();
    if (process_slot < 0)
        return -ENOMEM;

    return process_load_for_slot(filename, process, process_slot);
}

int process_switch(struct process* process) {
    current_process = process;

    return 0;
}

int process_load_switch(const char* filename, struct process** process) {
    int ret = 0;
    if ((ret = process_load(filename, process)) < 0)
        return ret;

    process_switch(*process);
    return 0;
}

static int process_find_free_allocatation_index(struct process* process) {
    int ret = -ENOMEM;
    for (int i = 0; i < PHIX_MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i] == 0)
            return i;
    }

    return ret;
}

void* process_malloc(struct process* process, size_t size) {
    int index;
    void* p = kzalloc(size);
    if (!p)
        return NULL;

    index = process_find_free_allocatation_index(process);
    if (index < 0)
        return NULL;

    process->allocations[index] = p;

    return p;
}

void process_free(struct process* process, void* p) {
    for (int i = 0; i < PHIX_MAX_PROGRAM_ALLOCATIONS; i++) {
        if (process->allocations[i] == p) {
            process->allocations[i] = NULL;
            kfree(p);
            return;
        }
    }

    /* TODO: Handle freeing invalid memory */
}
