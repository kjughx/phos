#include "gdt.h"
#include "../memory/memory.h"

#define GDT_SEGMENTS 3

struct gdt gdt_real[GDT_SEGMENTS];
struct gdt_structured gdt_structured[GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},       /* NULL Segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A}, /* Kernel code segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92}, /* Kernel data segment */
};

static void encode_gdt_entry(uint8_t* target, struct gdt_structured src) {
    if ((src.limit > 65536) && (src.limit & 0xFFF) != 0xFFF)
        panic();

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

void _gdt_init() {
    _memset(gdt_real, 0, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, GDT_SEGMENTS);

    /* Load the GDT */
    gdt_load(gdt_real, sizeof(gdt_real));
}
