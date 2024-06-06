#![no_std]
#![no_main]
#![feature(asm)]
#![feature(naked_functions)]
#![feature(once_cell)]

mod start;
mod memory;
mod idt;
mod tty;
mod types;

use idt::idt_init;
use tty::{init_screen, print};

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    init_screen();

    idt_init();

    print("Hello, World!");

    loop {}
}
