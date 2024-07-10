#![no_std]
#![no_main]

use core::hint;
use ruix::path;
use ruix::{println, trace};

use ruix::fs;
use ruix::gdt::gdt_init;
use ruix::idt::idt_init;
use ruix::tty::init_screen;

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    init_screen();

    gdt_init();
    idt_init();

    fs::resolve(ruix::disk::get_disk_mut(0)).ok().unwrap();

    let Ok(desc) = fs::open(path::Path::new("0:/HELLO"), fs::FileMode::ReadOnly) else {
        panic!("Failed to open file");
    };

    let mut buf: [u8; 256] = [0; 256];
    desc.read(256, 1, &mut buf).unwrap();

    trace!("{:#?}", buf);

    println!("Hello, World!");
    loop {
        hint::spin_loop()
    }
}
