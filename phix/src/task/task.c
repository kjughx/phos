#include "task/task.h"
#include "common.h"
#include "config.h"
#include "idt/idt.h"
#include "kernel.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "task/process.h"
#include <memory/heap/kheap.h>

/* Idle task for when there are no runnable tasks left */
struct task* idle_task;

/* Currently running task */
struct task* current_task;

/* Task linked list */
struct task* task_tail = NULL;
struct task* task_head = NULL;

struct task* task_current() { return current_task; }

static int create_idle_task() {
    idle_task = kzalloc(sizeof(struct task));

    if (!idle_task)
        panic("Failed to create idle task");

    if (!(idle_task->process = kzalloc(sizeof(struct process))))
        panic("Failed to create idle process");

    idle_task->process->p = "þë"; /* 0xfe 0xeb: jmp $ */
    idle_task->process->size = 2;
    idle_task->process->filetype = PROCESS_FILETYPE_BINARY;

    if (!(idle_task->page_directory =
              paging_new_directory(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL)))
        panic("Failed to allocate a page directory for idle_task");

    if (paging_map_to(idle_task->page_directory, (void*)PHIX_PROGRAM_VIRTUAL_ADDRESS,
                      idle_task->process->p,
                      (void*)PAGE_ALIGN(idle_task->process->p + idle_task->process->size),
                      PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL) < 0)
        panic("Failed to map idle_task page");

    idle_task->registers.ip = PHIX_PROGRAM_VIRTUAL_ADDRESS;
    idle_task->registers.ss = USER_DATA_SEGMENT;
    idle_task->registers.cs = USER_CODE_SEGMENT;
    idle_task->registers.esp = PHIX_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    current_task = idle_task;
    task_head = idle_task;
    task_tail = idle_task;
}

int copy_string_from_task(struct task* task, void* virtual, void* phys, int max) {
    int ret = 0;
    char* tmp = NULL;
    pte_t old_directory = 0;

    if (max >= PAGING_PAGE_SIZE)
        return -EINVAL;

    if (!(tmp = kzalloc(max)))
        return -ENOMEM;

    if ((ret = paging_get(task->page_directory, tmp, &old_directory)) < 0)
        return ret;

    /* Map tmp -> tmp for the task */
    paging_map(task->page_directory, tmp, tmp,
               PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);
    strncpy(tmp, virtual, max);
    kernel_page();

    if ((ret = paging_set(task->page_directory, tmp, old_directory)) < 0)
        goto out;

    strncpy(phys, tmp, max);

out:
    kfree(tmp);

    return ret;
}

int copy_string_to_task(struct task* task, void* phys, void* virtual, int max) {
    int ret = 0;
    char* tmp = NULL;
    pte_t old_directory = 0;

    if (max >= PAGING_PAGE_SIZE)
        return -EINVAL;

    if ((ret = paging_get(task->page_directory->directory_entry, phys, &old_directory)) < 0)
        return ret;

    /* Map tmp -> tmp for the task */
    paging_map(task->page_directory, phys, phys,
               PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);
    strncpy(virtual, phys, max);
    kernel_page();

    ret = paging_set(task->page_directory->directory_entry, tmp, old_directory);

    return ret;
}

int task_switch(struct task* task) {
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

void task_save_state(struct task* task, struct interrupt_frame* frame) {
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame* frame) {
    struct task* task;
    if (!(task = task_current()))
        panic("No current task to save");

    task_save_state(task, frame);
}

int task_page() {
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_page_task(struct task* task) {
    user_registers();
    paging_switch(task->page_directory);
    return 0;
}

void task_run_first_task() {
    /* Add an idle task for when there are no more tasks left */
    if (!create_idle_task())
        panic("Failed to create idle task");

    task_switch(task_head);
    task_return(&task_head->registers);
}

int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));

    if (!(task->page_directory = paging_new_directory(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL)))
        return -EIO;

    task->registers.ip = PHIX_PROGRAM_VIRTUAL_ADDRESS;
    if (process->filetype == PROCESS_FILETYPE_ELF)
        task->registers.ip = elf_header(process->elf_file)->e_entry;

    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = PHIX_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    task->process = process;

    return 0;
}

struct task* task_new(struct process* process) {
    int ret;
    struct task* task;

    if (!(task = kzalloc(sizeof(struct task))))
        return ERROR(-ENOMEM);

    if ((ret = task_init(task, process)) < 0)
        return ERROR(ret);

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

    return task;
}

struct task* task_get_next() {
    if (!current_task->next)
        return task_head;

    return current_task->next;
}

void task_switch_next() {
    struct task* next = task_get_next();
    if (!next)
        panic("No more tasks");

    task_switch(next);
    task_return(&next->registers);
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

    paging_free_chunk(task->page_directory);
    task_list_remove(task);
    kfree(task);
}

void* task_get_stack_item(struct task* task, int index) {
    void* ret = NULL;
    uint32_t* sp_ptr = (uint32_t*)task->registers.esp;
    /* Switch to the `task`'s page */
    task_page_task(task);
    ret = (void*)sp_ptr[index];

    /* Switch back to kernel page */
    kernel_page();
    return ret;
}
