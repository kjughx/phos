#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "config.h"

enum Syscalls {
    SYSCALL_PRINT,
    SYSCALL_GETKEY,
    SYSCALL_PUTCHAR,
    SYSCALL_MALLOC,
    SYSCALL_FREE,
    SYSCALL_EXEC,
    SYSCALL_EXIT,
    SYSCALL_OPEN,
    SYSCALL_READ,
    SYSCALL_CLOSE,
#ifdef DEBUG
    SYSCALL_GET_PADDR = PHIX_MAX_SYSCALLS - 2,
    SYSCALL_GET_FLAGS = PHIX_MAX_SYSCALLS - 1,
#endif
};

struct interrupt_frame;
void* syscall0_print(struct interrupt_frame* frame);
void* syscall1_getkey(struct interrupt_frame* frame);
void* syscall2_putchar(struct interrupt_frame* frame);
void* syscall3_malloc(struct interrupt_frame* frame);
void* syscall4_free(struct interrupt_frame* frame);
void* syscall5_exec(struct interrupt_frame* frame);
void* syscall6_exit(struct interrupt_frame* frame);
void* syscall7_open(struct interrupt_frame* frame);
void* syscall8_read(struct interrupt_frame* frame);
void* syscall9_close(struct interrupt_frame* frame);

#ifdef DEBUG
void* syscall1022_get_paddr(struct interrupt_frame* frame);
void* syscall1023_get_flags(struct interrupt_frame* frame);
#endif

void syscall_register_commands();

#endif /*_SYSCALL_H_ */
