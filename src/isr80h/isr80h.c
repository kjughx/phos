#include "isr80h/isr80h.h"
#include "idt/idt.h"
#include "isr80h/misc.h"

void isr80h_register_commands() {
    isr80h_register_command(SYSCALL_SUM, isr80h_command0_sum);
}
