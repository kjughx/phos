pub mod r#box;
pub mod r#dyn;
pub mod vec;

use crate::{
    memory::{Heap, HEAP_BLOCK_SIZE},
    sync::Global,
};

const KERNEL_HEAP_SIZE: usize = 100 * 1024 * 1024; // 100MB

pub static mut KERNEL_HEAP: Global<Heap> = Global::new(
    || {
        Heap::new(
            0x00007E00,
            KERNEL_HEAP_SIZE / HEAP_BLOCK_SIZE as usize,
            0x01000000,
        )
    },
    "KERNEL_HEAP",
);
