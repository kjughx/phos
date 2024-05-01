#ifndef _KERNEL_H_
#define _KERNEL_H_

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void panic(const char* message);

void kernel_main();

#endif /* _KERNEL_H_  */
