section .asm

extern no_interrupt_handler
extern interrupt_handler

global idt_load
global interrupt_pointer_table
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]

    pop ebp
    ret

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
