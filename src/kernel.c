#include <stddef.h>
#include <stdint.h>

#include "idt/idt.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "string/string.h"

void kernel_main() {
    terminal_init();
    print("Hello, World!");

    /* Initialize the kernel heap */
    kheap_init();

    /* Initialize the interrupt desciptor table */
    idt_init();

    /* Enable system interrupts*/
    enable_interrupts();

    /* Never return */
    for (;;)
        ;
}
