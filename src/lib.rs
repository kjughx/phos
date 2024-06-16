#![no_std]
#![feature(naked_functions)]
#![allow(internal_features)]
#![feature(ptr_internals)]
#![allow(dead_code)]

pub mod disk;
pub mod fs;
mod fs_impl;
pub mod gdt;
pub mod idt;
pub mod io;
pub mod memory;
pub mod serial;
pub mod start;
pub mod string;
pub mod tty;
pub mod types;

pub use memory::Box;
pub use memory::Dyn;

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
