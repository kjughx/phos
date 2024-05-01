#ifndef _IDT_H_
#define _IDT_H_
#include "common.h"

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

void idt_init();
void enable_interrupts();
void disable_interrupts();

#endif /* _IDT_H_ */
