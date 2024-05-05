#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "common.h"
#include "config.h"
#include "task/task.h"

struct process {
    /* The process id */
    uint16_t id;

    char filename[PHOS_MAX_PATH];

    /* The main process task */
    struct task* task;

    /* The memory malloc'd by the process */
    void* allocations[PHOS_MAX_PROGRAM_ALLOCATIONS];

    /* Physical pointer to process memory */
    void* p;

    /* Size of data pointed to by p */
    uint32_t size;

    /* Physical pointer to process stack */
    void* stack;

    struct keyboard_buffer {
        char buffer[PHOS_KEYBOARD_BUFFER_SIZE];
        int reader;
        int writer;
    } keyboard;
};

struct process* process_current();
int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);

#endif /* _PROCESS_H_ */
