#include "isr80h/memory.h"
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
