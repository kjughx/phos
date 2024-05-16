#include "common.h"
#include "fs/file.h"
#include "idt/idt.h"
#include "kernel.h"
#include "keyboard/keyboard.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "string/string.h"
#include "syscall/syscall.h"
#include "task/task.h"
#include "task/process.h"

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

void* syscall7_open(struct interrupt_frame* frame) {
    char filename[PHIX_MAX_PATH] = "0:/";
    char mode[5];
    int ret = 0;
    int fd = -1;

    if (task_current()->process->open_fd)
        return (void*)-EISTKN;

    void* user_filename = task_get_stack_item(task_current(), 1);
    void* user_mode = task_get_stack_item(task_current(), 0);

    ret = copy_string_from_task(task_current(), user_filename, filename + 3, sizeof(filename));
    if (ret < 0)
        return ERROR(ret);

    ret = copy_string_from_task(task_current(), user_mode, mode, sizeof(mode));
    if (ret < 0)
        return ERROR(ret);

    fd = fopen((const char*)filename, (const char*)mode);
    if (fd < 0)
        return ERROR(fd);

    task_current()->process->open_fd = fd;

    return (void*)fd;
}

/* int read(void* buf, size_t size, size_t count, int fd) */
void* syscall8_read(struct interrupt_frame* frame) {
    void* user_buf = task_get_stack_item(task_current(), 3);
    size_t size = (size_t)task_get_stack_item(task_current(), 2);
    size_t count = (size_t)task_get_stack_item(task_current(), 1);
    int fd = (int)task_get_stack_item(task_current(), 0);
    int ret = 0;

    if (size * count >= 1024)
        return (void*)-EINVAL;

    void* buf = kzalloc(size * count);
    ret = fread(buf, size, count, fd);
    if (ret < 0)
        goto out;

    ret = copy_string_to_task(task_current(), buf, user_buf, size * count);

out:
    kfree(buf);

    return (void*)ret;
}

void* syscall9_close(struct interrupt_frame* frame) {
    int user_fd = (int)task_get_stack_item(task_current(), 0);
    task_current()->process->open_fd = 0;

    return (void*)fclose(user_fd);
}
