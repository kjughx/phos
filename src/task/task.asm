[BITS 32]
section .asm

global task_return
task_return:
    mov ebp, esp
    ; PUSH DATA SEGMENT (SS WILL BE FINE)
    ; PUSH STACK_ADDRESS
    ; PUSH THE FLAGS
    ; PUSH THE CODE SEGMENT
    ; PUSH IP

    ; Access the structure passed to us
    mov ebx, [ebp+4]

    ; Push the data/stack selector
    push dword [ebx+44]
    ; Push the stack pointer

    push dword [ebx+40]
    ;Push the flags
    pushf

    pop eax
    or eax, 0x200
    push eax

    ; Push the code segment
    push dword [ebx+32]

    ; Push the IP to execute
    push dword [ebx+28]

    ; Segment registers
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebx+4]
    call restore_gpr

    add esp, 4

    ; Let's leave kernelland and start executing in userland
    iretd

global restore_gpr
restore_gpr: ; Restore the general purpose registers
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

global user_registers
user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ret
