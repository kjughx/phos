#include "isr80h/isr80h.h"
#include "idt/idt.h"
#include "isr80h/io.h"
#include "isr80h/memory.h"
#include "isr80h/misc.h"
#include "isr80h/process.h"

void isr80h_register_commands() {
    syscall_register(SYSCALL_SUM, syscall0_sum);
    syscall_register(SYSCALL_PRINT, syscall1_print);
    syscall_register(SYSCALL_GETKEY, syscall2_getkey);
    syscall_register(SYSCALL_PUTCHAR, syscall3_putchar);
    syscall_register(SYSCALL_MALLOC, syscall4_malloc);
    syscall_register(SYSCALL_FREE, syscall5_free);
    syscall_register(SYSCALL_EXEC, syscall6_process_load_start);
}
