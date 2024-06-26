pub mod r#box;
pub mod r#dyn;
pub mod vec;

use crate::{memory::Heap, sync::Global};

const KERNEL_HEAP_SIZE: usize = 100 * 1024 * 1024; // 100MB
const KERNEL_HEAP_START: usize = 0x01000000;
const KERNEL_ENTRIES_START: usize = 0x00007E00;

#[rustfmt::skip]
static mut KERNEL_HEAP: Global<Heap> = Global::new(
    || Heap::new(KERNEL_ENTRIES_START, KERNEL_HEAP_SIZE, KERNEL_HEAP_START),
    "KERNEL_HEAP",
);
