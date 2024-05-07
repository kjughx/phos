#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "common.h"
#include "config.h"

struct task;

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
    char filename[PHOS_MAX_PATH];
    struct task* task;
    void* allocations[PHOS_MAX_PROGRAM_ALLOCATIONS];
    void* p;
    uint32_t size;
    void* stack;
    struct keyboard_buffer {
        char buffer[PHOS_KEYBOARD_BUFFER_SIZE];
        int reader;
        int writer;
    } keyboard;
};

/* @brief Get the current process */
struct process* process_current();

/* @brief Load a new @process with the contens of @filename */
int process_load(const char* filename, struct process** process);

#endif /* _PROCESS_H_ */
