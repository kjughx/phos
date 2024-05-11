[BITS 32]

global _start
extern main
extern exit

section .asm

_start:
    call main
    push eax
    call exit
    ret
