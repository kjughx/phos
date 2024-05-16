#ifndef _PHIX_H_
#define _PHIX_H_

#include "stddef.h"
#include "stdint.h"

void phix_print(const char* message);
int phix_getkey();
void* phix_malloc(size_t size);
void phix_free(void* p);
void phix_putchar(char c);
void phix_exec(const char* filename);
[[noreturn]] void phix_exit(int status_code);
int phix_open(const char* filename, const char* mode);
int phix_read(void* buf, size_t count, size_t n, int fd);
int phix_close(int fd);

void* phix_get_paddr(void* vaddr);
uint8_t phix_get_flags(void* vaddr);

#endif /* _phix_H_ */
