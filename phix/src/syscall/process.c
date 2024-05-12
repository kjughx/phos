#include "syscall/syscall.h"
#include "config.h"
#include "idt/idt.h"
#include "kernel.h"
#include "string/string.h"
#include "task/process.h"
#include "task/task.h"

void* syscall5_exec(struct interrupt_frame* frame) {
    char filename[PHIX_MAX_PATH] = "0:/";
    void* filename_user_ptr = task_get_stack_item(task_current(), 0);
    struct process* new_process;

    int ret =
        copy_string_from_task(task_current(), filename_user_ptr, filename + 3, sizeof(filename));
    if (ret < 0)
        return ERROR(ret);

    ret = process_load_switch(filename, &new_process);
    if (ret < 0)
        return ERROR(ret);

    task_switch(new_process->task);
    task_return(&new_process->task->registers);
    /* Should not fall through */

    panic();
    return NULL;
}

void* syscall6_exit(struct interrupt_frame* frame) {
    process_terminate(task_current()->process);
    task_switch_next();
    return NULL;
}
