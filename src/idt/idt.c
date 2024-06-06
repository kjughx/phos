#include "../common.h"
#include "../memory/memory.h"
#include "../io/io.h"

#define ACK_INTR()                                                                                 \
    do {                                                                                           \
        outb(0x20, 0x20);                                                                          \
    } while (0);

#define MAX_INTERRUPTS 128

struct idt_desc {
    uint16_t offset_1; /* Offset 0-15 */
    uint16_t selector; /* Selector that's in our GDT */
    uint8_t zero;      /* Unused */
    uint8_t type_attr; /* Descriptor type and attributes */
    uint16_t offset_2; /* Offset 16-31 */
} __attribute__((packed));

struct idtr_desc {
    uint16_t limit; /* Size of descriptor table - 1*/
    uint32_t base;  /* Base address of start of interrupt descriptor table */
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

/* From idt.asm */
extern void* interrupt_pointer_table[MAX_INTERRUPTS];

struct idt_desc idt_descriptors[MAX_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* p);

void interrupt_handler(int interrupt, struct interrupt_frame* frame) {
    (void)interrupt;
    (void)frame;
    ACK_INTR();
}

void idt_set(int i, void* addr) {
    struct idt_desc* desc = &idt_descriptors[i];
    desc->offset_1 = (uint32_t)addr & 0x0000ffff;
    desc->selector = 0x08;
    desc->zero = 0;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)addr >> 16;
}

/* This is exported to rust */
void _idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for (int i = 0; i < MAX_INTERRUPTS; i++) {
        idt_set(i, interrupt_pointer_table[i]);
    }

    idt_load(&idtr_descriptor);
}
