#include "task/task.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include <memory/heap/kheap.h>
#include "task/process.h"

/* Currently running task */
struct task* current_task;

/* Task linked list */
struct task* task_tail = NULL;
struct task* task_head = NULL;

struct task* task_current() { return current_task; }

int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));

    if (!(task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL)))
        return -EIO;

    task->registers.pc = PHOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = PHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    task->process = process;

    return 0;
}

struct task* task_new(struct process* process) {
    int ret;
    struct task* task;

    if ((task = kzalloc(sizeof(struct task))))
        return ERROR(-ENOMEM);

    if ((ret = task_init(task, process)) < 0)
        return ERROR(ret);

    /* First and only task */
    if (!task_head) {
        task_head = task;
        task_tail = task;
        return task;
    }

    task_tail->next = task;
    task_tail = task;

    return task;
}

struct task* task_get_next() {
    if (!current_task->next)
        return task_head;

    return current_task->next;
}

static void task_list_remove(struct task* task) {
    if (task->prev)
        task->prev->next = task->next;

    if (task == task_head)
        task_head = task->next;

    if (task == task_tail)
        task_tail = task->prev;

    if (task == current_task)
        current_task = task_get_next();
}

void task_free(struct task* task) {
    if (!task)
        return;

    paging_free_4gb(task->page_directory);
    task_list_remove(task);
    kfree(task);
}
