section .asm

extern int21h_handler
extern isr80h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]

    pop ebp
    ret

int21h:
    pushad

    call int21h_handler

    popad
    iret

no_interrupt:
    pushad

    call no_interrupt_handler

    popad
    iret

isr80h_wrapper:
    cli
    ; INTERRUPT FRAME START ;
    ; Pushed to us by processor
    ; uint32_t ip
    ; uint32_t cs
    ; uint32_t flags
    ; uint32_t sp
    ; uint32_t ss

    ; Push general purpose registers to stack
    pushad

    ; INTERRUPT FRAME END ;

    ; Push the stack pointer so that we are pointing to the interrupt frame
    push esp

    ; Syscall number to the stack for the isr80h_handler
    push eax

    call isr80h_handler
    ;NOTE: eax holds the address returned by isr80h_handler

    add esp, 8 ; Equivalent to popping eax and esp

    ; Restore general purpose registers for user land
    popad
    iretd
