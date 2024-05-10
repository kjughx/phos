#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "common.h"
#include "config.h"
#include "loader/formats/elfloader.h"

typedef unsigned char PROCESS_FILETYPE;
#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

struct task;

struct process_allocation {
    size_t size;
    void* p;
};

/* @brief Represents a process
 *
 * @member id: The process id
 * @member filename: The path to the executable
 * @member task: The main process task
 * @member allocations: The memory alloc'd by the process
 * @member p:  Physical pointer to the process memory
 * @member size: Size of data pointed to by @p
 * @member stack: Physical pointer to process stack
 * @member keyboard: The keyboard buffer for the process
 */
struct process {
    uint16_t id;
    char filename[PHIX_MAX_PATH];
    struct task* task;
    struct process_allocation allocations[PHIX_MAX_PROGRAM_ALLOCATIONS];
    PROCESS_FILETYPE filetype;
    union {
        void* p; /* Binary file */
        struct elf_file* elf_file;
    };
    uint32_t size;
    struct bss {
        void* p;
        size_t size;
    } bss;
    void* stack;
    struct keyboard_buffer {
        char buffer[PHIX_KEYBOARD_BUFFER_SIZE];
        int reader;
        int writer;
    } keyboard;
};

/* @brief Get the current process */
struct process* process_current();

/* @brief Load a new @process with the contens of @filename */
int process_load(const char* filename, struct process** process);

/* @brief Switch current process to @process */
int process_switch(struct process* process);

/* @brief Load and switch current process to @procees */
int process_load_switch(const char* filename, struct process** process);

void* process_malloc(struct process* process, size_t size);
void process_free(struct process* process, void* p);

#endif /* _PROCESS_H_ */
