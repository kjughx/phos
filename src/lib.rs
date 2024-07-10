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

extern crate packed_macro;

pub mod boxed;
pub mod disk;
pub mod fs;
pub mod gdt;
pub mod idt;
pub mod io;
pub mod memory;
pub mod path;
pub mod prelude;
pub mod serial;
pub mod start;
pub mod string;

#[macro_use]
pub mod sync;
pub mod tty;

pub use prelude::*;

pub struct Addr(pub u32);
pub use packed_macro::{packed, Packed};

pub trait _Packed_: Sized {}

pub trait FromBytes: _Packed_ {
    type Output;
    fn from_bytes(bytes: &[u8]) -> Self::Output;
}

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
macro_rules! __print {
    ($($arg:tt)*) => {
        $crate::tty::print(format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! __println {
    () => ($crate::print!("\n"));
    ($fmt:expr) => ($crate::__print!(concat!($fmt, "\n")));
    ($fmt:expr, $($arg:tt)*) => ($crate::__print!(
        concat!($fmt, "\n"), $($arg)*));
}
