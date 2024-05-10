[BITS 32]
section .asm

global print:function
; void print(const char* message)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1 ; command print
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global getkey:function
; int getkey()
getkey:
    push ebp
    mov ebp, esp
    mov eax, 2 ; command print
    int 0x80   ; invoke syscall
    pop ebp
    ret

global phos_malloc:function
; void* phos_malloc(size_t size)
phos_malloc:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command malloc
    push dword[ebp+8] ; Variable size
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phos_free:function
; void phos_free(void* p)
phos_free:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command free
    push dword[ebp+8] ; Variable p
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret
