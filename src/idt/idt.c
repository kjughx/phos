#include "common.h"

#include "config.h"
#include "idt/idt.h"
#include "io/io.h"
#include "kernel.h"
#include "memory/memory.h"
#include "string/string.h"
#include "task/task.h"

#define ACK_INTR()                                                                                 \
    do {                                                                                           \
        outb(0x20, 0x20);                                                                          \
    } while (0);


struct idt_desc idt_descriptors[PHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* p);
extern void no_interrupt();
extern void isr80h_wrapper();

static ISR80H_COMMAND isr80h_commands[PHOS_MAX_ISR80H_COMMANDS];

extern void* interrupt_pointer_table[PHOS_TOTAL_INTERRUPTS];
void interrupt_handler(int interrupt, struct interrupt_frame* frame) {
    ACK_INTR();
}

void no_interrupt_handler() { ACK_INTR(); }

void isr80h_register_command(int command_id, ISR80H_COMMAND command) {
    /* Invalid command */
    if (command_id < 0 || command_id >= PHOS_MAX_ISR80H_COMMANDS)
        panic("Command ID is out of bounds");

    if (isr80h_commands[command_id])
        panic("Command already in use");

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame) {
    /* Invalid command */
    if (command < 0 || command >= PHOS_MAX_ISR80H_COMMANDS)
        return NULL;

    ISR80H_COMMAND func = isr80h_commands[command];
    if (!func)
        return NULL;

    return func(frame);
}

void* isr80h_handler(int command, struct interrupt_frame* frame) {
    void* ret = NULL;
    kernel_page();
    task_current_save_state(frame);
    ret = isr80h_handle_command(command, frame);
    task_page();
    ACK_INTR();
    return ret;
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

    for (int i = 0; i < PHOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, interrupt_pointer_table[i]);
    }

    idt_set(0x80, isr80h_wrapper);

    idt_load(&idtr_descriptor);
}
