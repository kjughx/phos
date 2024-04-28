ORG 0
BITS 16

; Dummy BIOS Parameter Block
bpb:
    jmp short step1
    nop

times 33 db 0

step1:
    jmp 0x7c0:step2

step2:
    cli ; Disable interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enable interrupts

    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop

.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10

    ret

times 510-($ - $$) db 0
dw 0xAA55
