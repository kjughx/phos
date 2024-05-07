#ifndef _IDT_H_
#define _IDT_H_
#include "common.h"

struct interrupt_frame;
typedef void* (*ISR80H_COMMAND)(struct interrupt_frame* frame);
typedef void (*INTERRUPT_CB_FUNCTION)(struct interrupt_frame* frame);

struct idt_desc {
    uint16_t offset_1; /* Offset 0-15 */
    uint16_t selector; /* Selector that's in our GDT */
    uint8_t zero;      /* Unused */
    uint8_t type_attr; /* Descriptor type and attributes */
    uint16_t offset_2; /* Offset 16-31 */
} __attribute__((packed));

struct idtr_desc {
    uint16_t limit; /* Size of descriptor table - 1*/
    uint32_t base;  /* bBase address of start of interrupt descriptor table */
} __attribute__((packed));

struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t unused;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

void idt_init();
void enable_interrupts();
void disable_interrupts();
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
int idt_register_intr_cb(int interrupt, INTERRUPT_CB_FUNCTION cb);

#endif /* _IDT_H_ */
