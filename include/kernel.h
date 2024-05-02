#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "string/string.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

static inline void panic(const char* message) {
    print("Kernel panic: ");
    print(message);
    asm volatile("hlt");
}

void kernel_main();

#define ERROR(p) ((void*)p)
#define ISERR(p) (((int)p) < 0)
#define PTR_ERR(p) ((int)p)

#endif /* _KERNEL_H_  */
