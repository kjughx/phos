#ifndef _SYSCALL_H_
#define _SYSCALL_H_

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

void syscall_register_commands();

#endif /*_SYSCALL_H_ */
