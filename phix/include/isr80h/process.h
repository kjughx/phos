#ifndef _ISR80H_PROCESS_H_
#define _ISR80H_PROCESS_H_

struct interrupt_frame;
void* syscall6_process_load_start(struct interrupt_frame* frame);

#endif /* _ISR80H_PROCESS_H_ */
