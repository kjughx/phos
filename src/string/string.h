#ifndef _STRING_H
#define _STRING_H

#include "common.h"

void terminal_putchar(int x, int y, char c, char color);
void terminal_init();

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max);
bool is_digit(char c);
int to_digit(char c);

void print(const char* str);

#endif /* _STRING_H*/
