#ifndef _TASK_H_
#define _TASK_H_

#include "common.h"
#include "memory/paging/paging.h"

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t pc;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct task {
    /* Page directory of the task */
    struct paging_4gb_chunck* page_directory;

    /* Registers of the task when it's not running */
    struct registers registers;

    /* Next task in the linked list */
    struct task* next;

    /* Previous task in the linked list */
    struct task* prev;
};

struct task* task_new();
struct task* task_current();
struct task* task_get_next();
void task_free(struct task* task);

#endif /* _TASK_H_ */
