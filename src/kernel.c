#include "kernel.h"
#include "boot/gdt/gdt.h"
#include "common.h"
#include "config.h"
#include "disk/disk.h"
#include "fs/file.h"
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "string/string.h"
#include "task/tss.h"

static struct paging_4gb_chunck* kchunk = NULL;
static struct tss tss;

struct gdt gdt_real[PHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                 /* NULL Segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},           /* Kernel code segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},           /* Kernel data segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf8},           /* User code segment */
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xf2},           /* User data segment */
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} /* TSS Segment  */
};

void kernel_main() {
    terminal_init();

    memset(gdt_real, 0, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, PHOS_TOTAL_GDT_SEGMENTS);

    /* Load the GDT */
    gdt_load(gdt_real, sizeof(gdt_real));

    /* Initialize the kernel heap */
    kheap_init();

    /* Initialize the file systems */
    fs_init();

    /* Search and initialize disks */
    disk_search_and_init();

    /* Initialize the interrupt desciptor table */
    idt_init();

    /* Setup the TSS */
    memset(&tss, 0, sizeof(tss));
    tss.esp = 0x600000; /* Kernel stack */
    tss.ss0 = KERNEL_DATA_SELECTOR;

    /* Load the TSS */
    tss_load(0x28);

    /* Setup a kernel paging chunk */
    kchunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /* Switch to kernel paging chunk */
    paging_switch(paging_4gb_chunk_get_directory(kchunk));

    /* Enable paging */
    enable_paging();

    /* Enable system interrupts*/
    enable_interrupts();

    print("Hello, World!");
    /* Never return */
    for (;;)
        ;
}
