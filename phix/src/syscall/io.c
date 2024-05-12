#include "common.h"
#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "string/string.h"
#include "syscall/syscall.h"
#include "task/task.h"

void* syscall0_print(struct interrupt_frame* frame) {
    void* user_msg_buf = task_get_stack_item(task_current(), 0);
    char* buf[512];
    copy_string_from_task(task_current(), user_msg_buf, buf, sizeof(buf));
    print((char*)buf);

    return NULL;
}

void* syscall1_getkey(struct interrupt_frame* frame) { return (void*)(int)keyboard_pop(); }

void* syscall2_putchar(struct interrupt_frame* frame) {
    char c = (char)(int)task_get_stack_item(task_current(), 0);
    putchar(c);

    return 0;
}
