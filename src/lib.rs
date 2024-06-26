#![no_std]
#![feature(naked_functions)]
#![feature(panic_info_message)]
#![allow(internal_features)]
#![feature(ptr_internals)]
#![feature(dispatch_from_dyn)]
#![feature(coerce_unsized)]
#![feature(deref_pure_trait)]
#![feature(unsize)]
#![allow(dead_code)]

pub mod boxed;
pub mod disk;
pub mod fs;
pub mod gdt;
pub mod idt;
pub mod io;
pub mod memory;
pub mod path;
pub mod serial;
pub mod start;
pub mod string;
pub mod sync;
pub mod tty;

pub use boxed::r#box::Box;
pub use boxed::r#dyn::Dyn;
pub use boxed::vec::{DynArray, Vec};

pub struct Addr(pub u32);

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

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {
        $crate::tty::print(format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($fmt:expr) => ($crate::print!(concat!($fmt, "\n")));
    ($fmt:expr, $($arg:tt)*) => ($crate::print!(
        concat!($fmt, "\n"), $($arg)*));
}
