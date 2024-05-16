#include "kernel.h"
#include "boot/gdt/gdt.h"
#include "common.h"
#include "config.h"
#include "disk/disk.h"
#include "fs/file.h"
#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "string/string.h"
#include "syscall/syscall.h"
#include "task/process.h"
#include "task/task.h"
#include "task/tss.h"

#define log_call(x, y)                                                                             \
    do {                                                                                           \
        printk(x "...");                                                                           \
        (y);                                                                                       \
        printk("done\n");                                                                          \
    } while (0);

static pgd_t* kernel_page_dir = NULL;

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_page_dir);
}

void kernel_main() {
    terminal_init();
    printk("PHOS v0.0.1\n");

    /* Initialize the GDT */
    log_call("Initializing the GDT", gdt_init());

    /* Initialize the kernel heap */
    log_call("Initializing the kheap", kheap_init());

    /* Initialize the file systems */
    log_call("Initializing the filsystems", fs_init());

    /* Search and initialize disks */
    log_call("Initializing the disks", disk_search_and_init());

    /* Initialize the interrupt desciptor table */
    log_call("Initializing the IDT", idt_init());

    /* Setup the TSS */
    log_call("Initializing the TSS", tss_init());

    /* Setup a kernel paging chunk */
    kernel_page_dir = paging_new_directory(PAGING_IS_WRITABLE | PAGING_IS_PRESENT);

    /* Switch to kernel paging chunk */
    paging_switch(kernel_page_dir);

    /* Enable paging */
    enable_paging();

    /* Register syscalls */
    log_call("Registering syscalls", syscall_register_commands());

    /* Initialize all keyboards */
    log_call("Initializing keyboards", keyboard_init());

    /* Enable system interrupts*/
    // enable_interrupts();
    struct process* process = NULL;
    if (process_load_switch("0:/shell", &process) < 0)
        panic("Failed to load shell");

    task_run_first_task();
    /* Never return */
    for (;;)
        ;
}
