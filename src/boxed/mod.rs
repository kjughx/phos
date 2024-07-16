pub mod arr;
pub mod r#box;
pub mod deque;
pub mod r#dyn;
pub mod list;
mod rawvec;
pub mod vec;

use crate::{memory::heap::Heap, Global};

const KERNEL_HEAP_SIZE: usize = 100 * 1024 * 1024; // 100MB
const KERNEL_HEAP_START: usize = 0x01000000;
const KERNEL_ENTRIES_START: usize = 0x00007E00;

static mut KERNEL_HEAP: Global<Heap> = Global::new(
    || Heap::new(KERNEL_ENTRIES_START, KERNEL_HEAP_SIZE, KERNEL_HEAP_START),
    "KERNEL_HEAP",
);
