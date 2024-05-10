#include "stdio.h"
#include "phos.h"
#include <stdarg.h>

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

int putchar(int c) {
    phos_putchar((char)c);
    return 0;
}

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

int printf(const char* fmt, ...) {
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
