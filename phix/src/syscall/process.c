#include "syscall/process.h"
#include "config.h"
#include "idt/idt.h"
#include "kernel.h"
#include "string/string.h"
#include "task/process.h"
#include "task/task.h"

void* syscall6_process_load_start(struct interrupt_frame* frame) {
    char filename[PHIX_MAX_PATH] = "0:/";
    void* filename_user_ptr = task_get_stack_item(task_current(), 0);
    struct process* new_process;

    int ret =
        copy_string_from_task(task_current(), filename_user_ptr, filename + 3, sizeof(filename));
    if (ret < 0)
        return ERROR(ret);

    ret = process_load_switch(sto_lower(filename), &new_process);
    if (ret < 0)
        return ERROR(ret);

    task_switch(new_process->task);
    task_return(&new_process->task->registers);
    /* Should not fall through */

    panic();
    return NULL;
}
