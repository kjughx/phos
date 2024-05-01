#ifndef _STRING_H
#define _STRING_H

#include "common.h"

void terminal_putchar(int x, int y, char c, char color);
void terminal_init();

size_t strlen(const char* str);

void print(const char* str);

#endif /* _STRING_H*/
