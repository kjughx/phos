#![no_std]
#![feature(asm)]
#![feature(naked_functions)]
#![feature(once_cell)]
#![feature(ptr_internals)]

pub mod start;
pub mod memory;
pub mod io;
pub mod idt;
pub mod gdt;
pub mod tty;
pub mod types;
pub mod disk;
pub mod fs;

pub use memory::Box as Box;
pub use memory::Dyn as Dyn;

#[macro_export]
macro_rules! spinwhile {
    ($cond:expr) => {
        while $cond {}
    };
}

#[macro_export]
macro_rules! spinuntil {
    ($cond:expr) => {
        while !($cond) {}
    };
}
