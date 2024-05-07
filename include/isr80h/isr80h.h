#ifndef _ISR80H_H_
#define _ISR80H_H_

enum Syscalls {
    SYSCALL_SUM,
    SYSCALL_PRINT,
    SYSCALL_GETKEY,
    SYSCALL_PUTCHAR,
};

void isr80h_register_commands();

#endif /*_ISR80H_H_ */
