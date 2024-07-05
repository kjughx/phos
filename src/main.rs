#![no_std]
#![no_main]

use core::hint;
use ruix::path;

use ruix::fs;
use ruix::gdt::gdt_init;
use ruix::idt::idt_init;
use ruix::println;
use ruix::tty::init_screen;

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    init_screen();

    gdt_init();
    idt_init();

    fs::resolve(ruix::disk::get_disk_mut(0)).ok().unwrap();

    assert!(fs::open(path::Path::new("0:/HELLO"), fs::FileMode::ReadOnly).is_ok());

    loop {
        hint::spin_loop()
    }
}
