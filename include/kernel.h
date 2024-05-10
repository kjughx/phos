#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "string/string.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

/* @brief Halt the system after printing @message */
static inline void panic(const char* message) {
    print("Kernel panic: ");
    print(message);
    print("\n");
    asm volatile("hlt");
}

/* @brief Restore the kernel registers */
void kernel_registers();

/* @brief The kernel entry main function */
void kernel_main();

/* @brief Switch to the kernel (one-to-one) page */
void kernel_page();

#define ERROR(p) ((void*)p)
#define ISERR(p) (((int)p) < 0)
#define PTR_ERR(p) ((int)p)

#endif /* _KERNEL_H_  */
