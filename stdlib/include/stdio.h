#ifndef _STDIO_H_
#define _STDIO_H_

#include "stdbool.h"
#include "stddef.h"

char* itoa(int i);
int atoi(const char* str);

int putchar(int c);
int printf(const char* fmt, ...);
int getkey();
void readline(char* buf, int max, bool output_while_typing);
int open(const char* filename, const char* mode);
int read(void* buf, size_t count, size_t n, int fd);
int close(int fd);

#endif /* _STDIO_H_ */
