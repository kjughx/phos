use core::{alloc::{GlobalAlloc, Layout, LayoutErr}, ptr};

const HEAP_BLOCK_SIZE: usize = 4096;
const BLOCK_FREE: u8 = 0x00;
const BLOCK_TAKEN: u8 = 0x01;

enum MemoryError {
    Generic,
}

type Addr = u32;

struct Heap {
    entries: *mut u8,
    count: usize,
    start: Addr,
}

impl Heap {
    pub fn new(entries: u32, count: usize, start: u32) -> Self {
        let entries = entries as *mut u8;
        unsafe { ptr::write_bytes(entries, BLOCK_FREE, count) };
        Self {
            entries,
            count,
            start,
        }
    }

    fn block_to_addr(&self, block: u32) -> Addr {
        return self.start + block * HEAP_BLOCK_SIZE as u32;
    }
    fn addr_to_block(&self, addr: Addr) -> u32 {
        assert!(addr > self.start);
        return (addr - self.start) / (HEAP_BLOCK_SIZE as u32);
    }
    fn mark_blocks_taken(&mut self, start_block: u32, total_blocks: u32) {

    }
}

struct Allocator;

unsafe impl GlobalAlloc for Allocator {
    unsafe fn alloc(&self, _layout: Layout) -> *mut u8 {
        todo!()
    }
    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
        todo!()
    }
}

#[global_allocator]
static ALLOCATOR: Allocator = Allocator;
