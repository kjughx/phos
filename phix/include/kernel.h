#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "string/string.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define panic(msg, ...)                                                                            \
    do {                                                                                           \
        printk("PANIC: [%s:%d]: " msg "\n", __func__, __LINE__, ##__VA_ARGS__);                    \
        asm volatile("hlt");                                                                       \
    } while (0);

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
