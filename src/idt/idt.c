#include <stdint.h>

#include "config.h"
#include "idt.h"
#include "memory/memory.h"
#include "string/string.h"

struct idt_desc idt_descriptors[PHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* p);

void idt_zero() { print("Divide by zero error\n"); }

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

    idt_set(1, idt_zero);

    idt_load(&idtr_descriptor);
}
