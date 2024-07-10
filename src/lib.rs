#![no_std]
#![feature(naked_functions)]
#![allow(internal_features)]
#![feature(ptr_internals)]
#![feature(dispatch_from_dyn)]
#![feature(coerce_unsized)]
#![feature(deref_pure_trait)]
#![feature(unsize)]
#![allow(dead_code)]

extern crate packed_macro;

pub mod boxed;
pub mod cpu;
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
pub mod task;

#[macro_use]
pub mod sync;
pub mod tty;

pub use prelude::*;

pub use packed_macro::{packed, Packed};

pub trait _Packed_: Sized {}

pub trait FromBytes: _Packed_ {
    type Output;
    fn from_bytes(bytes: &[u8]) -> Self::Output;
}

pub enum Error {
    InvalidArgument,
    Unknown,
}
