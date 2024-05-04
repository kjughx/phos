#include "common.h"

#include "config.h"
#include "idt/idt.h"
#include "io/io.h"
#include "kernel.h"
#include "memory/memory.h"
#include "string/string.h"
#include "task//task.h"

#define ACK_INTR()                                                                                 \
    do {                                                                                           \
        outb(0x20, 0x20);                                                                           \
    } while (0);

struct idt_desc idt_descriptors[PHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void int21h();
extern void idt_load(struct idtr_desc* p);
extern void no_interrupt();

void int21h_handler() {
    print("Keyboard pressed!\n");
    ACK_INTR();
}

void no_interrupt_handler() { ACK_INTR(); }

void* isr80h_handle_command(int command, struct interrupt_frame* frame) { return NULL; }

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
        idt_set(i, no_interrupt);
    }

    idt_set(0x21, int21h);

    idt_load(&idtr_descriptor);
}
