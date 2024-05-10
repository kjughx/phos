#include "isr80h/memory.h"
#include "memory/paging/paging.h"
#include "task/process.h"
#include "task/task.h"

void* syscall4_malloc(struct interrupt_frame* frame) {
    size_t size = (size_t)task_get_stack_item(task_current(), 0);

    return process_malloc(task_current()->process, size);
}

void* syscall5_free(struct interrupt_frame* frame) {
    void* p = (void*)task_get_stack_item(task_current(), 0);
    process_free(task_current()->process, p);

    return 0;
}

void* syscall7_get_paddr(struct interrupt_frame* frame) {
    void* p = (void*)task_get_stack_item(task_current(), 0);
    return (void*) paging_get_paddr(task_current()->page_directory->directory_entry, p);
}

void* syscall8_get_flags(struct interrupt_frame* frame) {
    void* p = (void*)task_get_stack_item(task_current(), 0);
    return (void*) (int)paging_get_flags(task_current()->page_directory->directory_entry, p);
}
