#include "keyboard/keyboard.h"
#include "common.h"
#include "keyboard/ps2.h"
#include "status.h"
#include "string/string.h"
#include "task/process.h"
#include "task/task.h"

static struct keyboard* keyboards_head = NULL;
static struct keyboard* keyboards_tail = NULL;

int keyboard_insert(struct keyboard* keyboard) {
    if (!keyboard->init)
        return -EINVAL;

    if (keyboards_tail) {
        keyboards_tail->next = keyboard;
    }

    keyboards_head = keyboard;
    keyboards_tail = keyboard;

    return keyboard->init();
}

void keyboard_init() { keyboard_insert(ps2_init()); }

static inline int keyboard_write_index(struct process* process) {
    return process->keyboard.writer % sizeof(process->keyboard.buffer);
}

static inline int keyboard_read_index(struct process* process) {
    return process->keyboard.reader % sizeof(process->keyboard.buffer);
}

void keyboard_backspace(struct process* process) {
    int index = keyboard_write_index(process);
    process->keyboard.writer -= 1;
    process->keyboard.buffer[index] = 0x00;
}

void keyboard_push(char c) {
    struct process* process = process_current();
    if (!process)
        return;

    if (c == 0x00)
        return;

    if (!process->keyboard.caps && isalpha(c))
        c += 32;

    process->keyboard.buffer[keyboard_write_index(process)] = c;
    process->keyboard.writer++;
}

void keyboard_shift(bool released) {
    struct process* process = process_current();
    if (!process)
        return;

    process->keyboard.caps = !released;
}

void keyboard_capslock() {
    struct process* process = process_current();
    if (!process)
        return;

    process->keyboard.caps ^= 1;
}

char keyboard_pop() {
    struct process* process = NULL;
    char c = 0;
    if (!task_current())
        return 0;

    process = task_current()->process;
    c = process->keyboard.buffer[keyboard_read_index(process)];
    if (c == 0x00)
        return 0;

    process->keyboard.buffer[keyboard_read_index(process)] = 0x00;
    process->keyboard.reader++;

    return c;
}
