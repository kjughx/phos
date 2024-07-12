#![no_std]
#![no_main]

use core::hint;

use ruix::{
    disk::Disk, fs::Vfs, gdt::GDT, idt::IDT, memory::paging::KernelPage, println,
    tty::terminal::Terminal,
};

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    // Init and clear the terminal
    Terminal::init();

    // Setup Global Descriptor Table
    GDT::load();

    // Setup Interrupt descriptor Table
    // FIXME: Use rust version of idt not C
    IDT::init();

    // Resolve the connected disks
    match Vfs::resolve(Disk::get_mut(0)) {
        Ok(()) => (),
        Err(_) => println!("Could not resolve disk 0"),
    }

    let kerneldirectory = KernelPage::load();

    println!("Hello, World!");
    loop {
        hint::spin_loop()
    }
}
