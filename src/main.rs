#![no_std]
#![no_main]

use core::hint;

use ruix::{disk::Disk, fs::Vfs, gdt::Gdt, idt::Idt, println, tty::terminal::Terminal};

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    // Init and clear the terminal
    Terminal::init();

    // Setup Global Descriptor Table
    Gdt::init();

    // Setup Interrupt descriptor Table
    Idt::init();

    // Resolve the connected disks
    match Vfs::resolve(Disk::get_mut(0)) {
        Ok(()) => (),
        Err(_) => println!("Could not resolve disk 0"),
    }

    println!("Hello, World!");
    loop {
        hint::spin_loop()
    }
}
