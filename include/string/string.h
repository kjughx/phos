#ifndef _STRING_H
#define _STRING_H

#include "common.h"

void terminal_putchar(int x, int y, char c, char color);
void terminal_init();

int to_lower(unsigned char c);
bool is_digit(char c);
int to_digit(char c);

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max);
char* strcpy(char* dest, const char* src);
char* strcpy_strip(char* dest, const char* src);
int strncmp(const char* s1, const char* s2, size_t n);
int istrncmp(const char* s1, const char* s2, size_t n);
int strnlen_terminator(const char* str, int max, char terminator);

void print(const char* str);

#endif /* _STRING_H*/
