#ifndef _ISR80H_H_
#define _ISR80H_H_

enum Syscalls {
    SYSCALL_SUM,
    SYSCALL_PRINT,
    SYSCALL_GETKEY,
    SYSCALL_PUTCHAR,
    SYSCALL_MALLOC,
    SYSCALL_FREE,
    SYSCALL_EXEC,
    SYSCALL_GET_PADDR,
    SYSCALL_GET_FLAGS,
};

void isr80h_register_commands();

#endif /*_ISR80H_H_ */
