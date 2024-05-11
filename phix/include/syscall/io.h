#ifndef _ISR80H_IO_H_
#define _ISR80H_IO_H_
struct interrupt_frame;

void* syscall1_print(struct interrupt_frame* frame);
void* syscall2_getkey(struct interrupt_frame* frame);
void* syscall3_putchar(struct interrupt_frame* frame);
void* syscall4_malloc(struct interrupt_frame* frame);

#endif /* _ISR80H_IO_H_ */
