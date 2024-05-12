#include "boot/gdt/gdt.h"
#include "common.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "task/tss.h"

static struct tss tss;

struct gdt gdt_real[PHIX_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PHIX_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                 /* NULL Segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},           /* Kernel code segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},           /* Kernel data segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8},           /* User code segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2},           /* User data segment */
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} /* TSS Segment  */
};

void gdt_load(struct gdt* gdt, int size);
void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt,
                           int total_entries);

void gdt_init() {
    memset(gdt_real, 0, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, PHIX_TOTAL_GDT_SEGMENTS);

    /* Load the GDT */
    gdt_load(gdt_real, sizeof(gdt_real));
}

void tss_init() {
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    /* Load the TSS */
    tss_load(0x28);
}

static void encode_gdt_entry(uint8_t* target, struct gdt_structured src) {
    if ((src.limit > 65536) && (src.limit & 0xFFF) != 0xFFF)
        panic("Invalid argument");

    target[6] = 0x40;

    if (src.limit > 65536) {
        src.limit >>= 12;
        target[6] = 0xC0;
    }

    /* Encode the limit */
    target[0] = src.limit & 0xFF;
    target[1] = (src.limit >> 8) & 0xFF;
    target[6] |= (src.limit >> 16) & 0x0F;

    /* Encode the base */
    target[2] = src.base & 0xFF;
    target[3] = (src.base >> 8) & 0xFF;
    target[4] = (src.base >> 16) & 0xFF;
    target[7] = (src.base >> 24) & 0xFF;

    /* Set the type */
    target[5] = src.type;
}

void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt,
                           int total_entries) {
    for (int i = 0; i < total_entries; i++) {
        encode_gdt_entry((uint8_t*)&gdt[i], structured_gdt[i]);
    }
}
