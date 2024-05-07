[BITS 32]
section .asm

global _start
_start:
    ; push message
    ; mov eax, 1
    ; int 0x80
    ; add esp, 4

_loop:
    call getkey
    push eax
    mov eax, 3 ; command putchar
    int 0x80
    add esp, 4

    jmp _loop

getkey:
    mov eax, 2 ; command getkey
    int 0x80
    cmp eax, 0x00
    je getkey
    ret

section .data
message: db "Hello, World! - Userland", 0xA, 0
