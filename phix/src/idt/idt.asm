section .asm

extern isr80h_handler
extern no_interrupt_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

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

no_interrupt:
    pushad

    call no_interrupt_handler

    popad
    iret

%macro interrupt 1
    global int%1
    int%1:
        ; INTERRUPT FRAME START ;
        ; Pushed to us by the processor
        ; uint32_t ip
        ; uint32_t cs
        ; uint32_t flags
        ; uint32_t sp
        ; uint32_t ss

        ; Push general purpose registers to stack
        pushad
        ; INTERRUPT FRAME END ;

        push esp
        push dword %1
        call interrupt_handler
        add esp, 8 ; pop the stack
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

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
    mov dword[tmp_res], eax

    add esp, 8 ; Equivalent to popping eax and esp

    ; Restore general purpose registers for user land
    popad

    mov eax, [tmp_res]
    iretd

section .data

tmp_res: dd 0

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep
