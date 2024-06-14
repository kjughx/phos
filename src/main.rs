#![no_std]
#![no_main]

use core::hint;

use ruix::fs;
use ruix::gdt::gdt_init;
use ruix::idt::idt_init;
use ruix::tty::{init_screen, print};

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    init_screen();

    gdt_init();
    idt_init();

    fs::resolve();
    print("Hello, World!");

    loop {
        hint::spin_loop()
    }
}
