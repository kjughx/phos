#ifndef _STDIO_H_
#define _STDIO_H_

#include "stddef.h"
#include "stdbool.h"

char* itoa(int i);
int putchar(int c);
int printf(const char* fmt, ...);
int getkey();
void readline(char* buf, int max, bool output_while_typing);

#endif /* _STDIO_H_ */
