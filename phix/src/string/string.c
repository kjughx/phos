#include "string/string.h"
#include "common.h"
#include "kernel.h"
#include <stdarg.h>

uint16_t* video_mem = 0;
static uint16_t terminal_row = 0;
static uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color) { return (color << 8 | c); }

bool is_digit(char c) { return (c >= 48 && c <= 57); }

char to_lower(unsigned char c) {
    if (c >= 65 && c <= 90)
        return c + 32;

    return c;
}

char to_upper(unsigned char c) {
    if (c >= 97 && c <= 122)
        return c - 32;

    return c;
}

int to_digit(char c) {
    if (!is_digit(c))
        return c;

    return c - 48;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }

    return len;
}

char* sto_lower(char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        str[i] = to_lower(str[i]);
    }

    return str;
}

char* sto_upper(char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        str[i] = to_upper(str[i]);
    }

    return str;
}

size_t strnlen(const char* str, size_t max) {
    size_t len = 0;
    while (str[len] && len != max) {
        len++;
    }

    return len;
}

char* strcpy(char* dest, const char* src) {
    char* res = dest;

    while (*src)
        *(dest++) = *(src++);

    *dest = 0;

    return res;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* res = dest;

    while (*src && n-- > 1)
        *(dest++) = *(src++);

    *dest = 0;

    return res;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    unsigned char u1, u2;
    while (n-- > 0) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            break;
    }

    return 0;
}

char* strcpy_strip(char* dest, const char* src) {
    char* res = dest;

    while (*src && *src != 0x20)
        *(dest++) = *(src++);

    *dest = 0;

    return res;
}

int strnlen_terminator(const char* str, int max, char terminator) {
    for (int i = 0; i < max; i++) {
        if (str[i] == '\0' || str[i] == terminator)
            return i;
    }

    return max;
}

int istrncmp(const char* s1, const char* s2, size_t n) {
    unsigned char u1, u2;
    while (n-- > 0) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (to_lower(u1) != to_lower(u2))
            return u1 - u2;
        if (u1 == '\0')
            break;
    }

    return 0;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_backspace() {
    if (terminal_col == 0 && terminal_row == 0)
        return;

    if (terminal_col == 0) {
        terminal_row--;
        terminal_col = VGA_WIDTH;
    }

    terminal_col--;
    putchar(' ');
    terminal_col--;
}

void terminal_writechar(char c, char color) {
    if (c == 0x08) {
        terminal_backspace();
        return;
    }

    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_init() {
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void print(const char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }
}

void putchar(char c) { terminal_writechar(c, 15); }

static char* decimal_to_hex(unsigned int decimal) {
    static char hex[12];
    int j = 11;

    while (decimal != 0) {
        int remainder = decimal % 16;
        if (remainder < 10)
            hex[--j] = 48 + remainder;
        else
            hex[--j] = 55 + remainder;

        decimal /= 16;
    }

    return &hex[j];
}

char* itoa(int i) {
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;
    if (i >= 0) {
        neg = 0;
        i = -i;
    }
    while (i) {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11)
        text[--loc] = '0';

    if (neg)
        text[--loc] = '-';

    return &text[loc];
}

int printk(const char* fmt, ...) {
    va_list ap;
    int ival;
    char* sval;

    va_start(ap, fmt);
    for (const char* p = fmt; *p; p++) {
        if (*p != '%') {
            putchar(*p);
            continue;
        }
        switch (*++p) {
        case 'd': {
            ival = va_arg(ap, int);
            print(itoa(ival));
        } break;
        case 'c': {
            ival = va_arg(ap, int);
            putchar(ival);
        } break;
        case 's': {
            sval = va_arg(ap, char*);
            print(sval);
        } break;
        case 'p': {
            ival = va_arg(ap, int);
            print("0X");
            print(decimal_to_hex(ival));
        } break;
        default:
            putchar(*p);
        }
        va_end(ap);
    }

    return 0;
}
