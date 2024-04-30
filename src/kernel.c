#include <stddef.h>
#include <stdint.h>

#include "idt/idt.h"
#include "kernel.h"
#include "string/string.h"

extern void problem();

void kernel_main() {
    terminal_init();
    print("Hello, World!");

    /* Initialize the interrupt desciptor table */
    idt_init();

    problem();

    /* Never return */
    for (;;) {
    }
}
