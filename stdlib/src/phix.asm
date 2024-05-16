[BITS 32]
section .asm

global phix_print:function
; void phix_print(const char* message)
phix_print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 0 ; command print
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_getkey:function
; int phix_getkey()
phix_getkey:
    push ebp
    mov ebp, esp
    mov eax, 1 ; command print
    int 0x80   ; invoke syscall
    pop ebp
    ret

global phix_putchar:function
; void phix_putchar(char c)
phix_putchar:
    push ebp
    mov ebp, esp
    mov eax, 2 ; command putchar
    push dword[ebp+8] ; Variable c
    int 0x80
    add esp, 4
    pop ebp
    ret

global phix_malloc:function
; void* phix_malloc(size_t size)
phix_malloc:
    push ebp
    mov ebp, esp
    mov eax, 3 ; Command malloc
    push dword[ebp+8] ; Variable size
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_free:function
; void phix_free(void* p)
phix_free:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command free
    push dword[ebp+8] ; Variable p
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_exec:function
; void phix_exec(const char* filename)
phix_exec:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command free
    push dword[ebp+8] ; Variable filename
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_exit:function
; void phix_exit(int status_code)
phix_exit:
    push ebp
    mov ebp, esp
    mov eax, 6 ; Command free
    push dword[ebp+8] ; Variable status_code
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_open:function
; int phix_open(const char* filename, const char* mode)
phix_open:
    push ebp
    mov ebp, esp
    mov eax, 7 ; Command free
    push dword[ebp+8] ; Variable filename
    push dword[ebp+12]; Variable mode
    int 0x80   ; invoke syscall
    add esp, 8
    pop ebp
    ret

global phix_read:function
; int phix_read(void* buf, size_t count, size_t n, int fd);
phix_read:
    push ebp
    mov ebp, esp
    mov eax, 8 ; Command free
    push dword[ebp+8] ; Variable buf
    push dword[ebp+12]; Variable count
    push dword[ebp+16]; Variable n
    push dword[ebp+20]; Variable fd
    int 0x80   ; invoke syscall
    add esp, 16
    pop ebp
    ret

global phix_close:function
; int phix_close(int fd)
phix_close:
    push ebp
    mov ebp, esp
    mov eax, 9 ; Command free
    push dword[ebp+8] ; Variable fd
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret


;; FOR DEBUGGING ;;

global phix_get_paddr:function
; void* phix_get_paddr(void* vaddr)
phix_get_paddr:
    push ebp
    mov ebp, esp
    mov eax, 7 ; Command free
    push dword[ebp+8] ; Variable vaddr
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret

global phix_get_flags:function
; uint8_t phix_get_flags(void* vaddr)
phix_get_flags:
    push ebp
    mov ebp, esp
    mov eax, 8 ; Command free
    push dword[ebp+8] ; Variable vaddr
    int 0x80   ; invoke syscall
    add esp, 4
    pop ebp
    ret
