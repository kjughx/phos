#include "syscall/syscall.h"
#include "idt/idt.h"

void syscall_register_commands() {
    syscall_register(SYSCALL_PRINT, syscall0_print);
    syscall_register(SYSCALL_GETKEY, syscall1_getkey);
    syscall_register(SYSCALL_PUTCHAR, syscall2_putchar);
    syscall_register(SYSCALL_MALLOC, syscall3_malloc);
    syscall_register(SYSCALL_FREE, syscall4_free);
    syscall_register(SYSCALL_EXEC, syscall5_exec);
    syscall_register(SYSCALL_EXIT, syscall6_exit);

#ifdef DEBUG
    syscall_register(SYSCALL_GET_PADDR, syscall1022_get_paddr);
    syscall_register(SYSCALL_GET_FLAGS, syscall1023_get_flags);
#endif
}
