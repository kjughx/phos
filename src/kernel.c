#include <stddef.h>
#include <stdint.h>

#include "idt/idt.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "string/string.h"

static struct paging_4gb_chunck* kchunk = NULL;

void kernel_main() {
    terminal_init();

    /* Initialize the kernel heap */
    kheap_init();

    /* Initialize the interrupt desciptor table */
    idt_init();

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

void panic(const char* message) {
    print("Kernel panic: ");
    print(message);
    for (;;)
        ;
}
