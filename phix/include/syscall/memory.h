#ifndef _ISR80H_MEMORY_H_
#define _ISR80H_MEMORY_H_

struct interrupt_frame;
void* syscall4_malloc(struct interrupt_frame* frame);
void* syscall5_free(struct interrupt_frame* frame);
void* syscall7_get_paddr(struct interrupt_frame* frame);
void* syscall8_get_flags(struct interrupt_frame* frame);

#endif /* _ISR80H_MEMORY_H_ */
