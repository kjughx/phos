#include "isr80h/io.h"
#include "common.h"
#include "string/string.h"
#include "task/task.h"

void* isr80h_command1_print(struct interrupt_frame* frame) {
    void* user_msg_buf = task_get_stack_item(task_current(), 0);
    char* buf[512];
    copy_string_from_task(task_current(), user_msg_buf, buf, sizeof(buf));
    print((char*)buf);

    return NULL;
}
