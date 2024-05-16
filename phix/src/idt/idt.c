#include "idt/idt.h"
#include "common.h"
#include "config.h"
#include "io/io.h"
#include "kernel.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "status.h"
#include "string/string.h"
#include "task/process.h"
#include "task/task.h"

#define ACK_INTR()                                                                                 \
    do {                                                                                           \
        outb(0x20, 0x20);                                                                          \
    } while (0);

/* From idt.asm */
extern void* interrupt_pointer_table[PHIX_TOTAL_INTERRUPTS];

struct idt_desc idt_descriptors[PHIX_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* p);
extern void syscall_wrapper();

static INTERRUPT_CB_FUNCTION interrupt_callbacks[PHIX_TOTAL_INTERRUPTS];
static SYSCALL syscalls[PHIX_MAX_SYSCALLS];

void interrupt_handler(int interrupt, struct interrupt_frame* frame) {
    kernel_page();
    if (interrupt_callbacks[interrupt]) {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt](frame);
    }
    task_page();

    ACK_INTR();
}

int idt_register_intr_cb(int interrupt, INTERRUPT_CB_FUNCTION cb) {
    if (interrupt < 0 || interrupt >= PHIX_TOTAL_INTERRUPTS)
        return -EINVAL;

    interrupt_callbacks[interrupt] = cb;

    return 0;
}

void no_interrupt_handler() { ACK_INTR(); }

void syscall_register(int command_id, SYSCALL command) {
    /* Invalid command */
    if (command_id < 0 || command_id >= PHIX_MAX_SYSCALLS)
        panic("Command ID is out of bounds");

    if (syscalls[command_id])
        panic("Command already in use");

    syscalls[command_id] = command;
}

void* syscall_handle_command(int command, struct interrupt_frame* frame) {
    /* Invalid command */
    if (command < 0 || command >= PHIX_MAX_SYSCALLS)
        return NULL;

    SYSCALL func = syscalls[command];
    if (!func)
        return NULL;

    return func(frame);
}

void* syscall_handler(int command, struct interrupt_frame* frame) {
    void* ret = NULL;
    kernel_page();
    task_current_save_state(frame);
    ret = syscall_handle_command(command, frame);
    task_page();
    ACK_INTR();
    return ret;
}

void exception_handler(struct interrupt_frame* frame) {
    printk("EXCEPTION\n");
    process_terminate(task_current()->process);
    task_switch_next();
}

void idt_timer() {
    ACK_INTR();
    task_switch_next();
}

void idt_set(int i, void* addr) {
    struct idt_desc* desc = &idt_descriptors[i];
    desc->offset_1 = (uint32_t)addr & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)addr >> 16;
}

void idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for (int i = 0; i < PHIX_TOTAL_INTERRUPTS; i++) {
        idt_set(i, interrupt_pointer_table[i]);
    }

    /* All the exceptions */
    for (int i = 0; i < 20; i++) {
        idt_register_intr_cb(i, exception_handler);
    }

    // idt_set(0x20, idt_timer);
    idt_set(0x80, syscall_wrapper);

    idt_load(&idtr_descriptor);
}
