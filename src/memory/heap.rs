use core::ptr::{self, Unique};

pub const HEAP_BLOCK_SIZE: u32 = 4096;

const BLOCK_FREE: u8 = 1 << 0;
const BLOCK_TAKEN: u8 = 1 << 1;
const BLOCK_FIRST: u8 = 1 << 4;
const BLOCK_HAS_NEXT: u8 = 1 << 5;

#[derive(Debug)]
enum MemoryError {
    NoAction,
    InvalidAlignment,
    OutOfMemory,
    Other,
}

pub type Addr = *mut u8;

pub struct Heap {
    entries: Unique<[u8]>,
    count: usize,
    start: Addr,
}

impl Heap {
    pub fn new(entries: u32, count: usize, start: u32) -> Self {
        if count == 0 {
            panic!("Count is 0");
        }

        if start % HEAP_BLOCK_SIZE != 0 {
            panic!("start % HEAP_BLOCK_SIZE");
        }

        let entries = unsafe {
            Unique::new_unchecked(ptr::slice_from_raw_parts_mut(entries as *mut u8, count))
        };

        unsafe { ptr::write_bytes(entries.as_ptr() as *mut u8, BLOCK_FREE, count) };

        Self {
            entries,
            count,
            start: start as Addr,
        }
    }

    fn block_to_addr(&self, block: usize) -> Addr {
        (self.start as u32 + block as u32 * HEAP_BLOCK_SIZE) as Addr
    }
    fn addr_to_block(&self, addr: Addr) -> usize {
        assert!(addr > self.start);
        (addr as usize - self.start as usize) / (HEAP_BLOCK_SIZE as usize)
    }

    fn mark_blocks_taken(&mut self, start_block: usize, total_blocks: usize) {
        if total_blocks == 1 {
            unsafe {
                self.entries.as_mut()[start_block] = BLOCK_TAKEN | BLOCK_FIRST;
            }
            return;
        }

        unsafe {
            self.entries.as_mut()[start_block] = BLOCK_TAKEN | BLOCK_FIRST | BLOCK_HAS_NEXT;
            ptr::write_bytes(
                (self.entries.as_ptr() as *mut u8).offset(start_block as isize + 1),
                BLOCK_TAKEN | BLOCK_HAS_NEXT,
                total_blocks - 2,
            );
            self.entries.as_mut()[start_block + total_blocks - 1] = BLOCK_TAKEN | BLOCK_FIRST;
        }
    }

    fn mark_blocks_free(&mut self, start_block: usize) {
        for i in start_block..self.count {
            let entry = unsafe {
                let _entry = self.entries.as_ref()[i];
                self.entries.as_mut()[i] = BLOCK_FREE;
                _entry
            };

            if entry & BLOCK_HAS_NEXT == 0 {
                break;
            }
        }
    }

    fn entry_type(entries: Unique<[u8]>, offset: usize) -> u8 {
        unsafe { entries.as_ref()[offset] & 0x0f }
    }

    fn get_free_block(&self, count: usize) -> Result<usize, MemoryError> {
        let mut bc = 0;
        let mut bs: isize = -1;

        for i in 0..self.count {
            if Self::entry_type(self.entries, i) != BLOCK_FREE {
                bc = 0;
                bs = -1;
                continue;
            }

            if bs == -1 {
                bs = i as isize;
            }

            bc += 1;
            if bc == count {
                break;
            }
        }
        if bs == -1 {
            return Err(MemoryError::OutOfMemory);
        }

        Ok(bs as usize)
    }

    fn alloc_blocks(&mut self, block_count: usize) -> *mut u8 {
        let _s = self.get_free_block(block_count).unwrap();

        let addr = self.block_to_addr(_s);
        self.mark_blocks_taken(_s, block_count);

        addr
    }

    fn copy_block(&mut self, src: usize, dst: usize) {
        let src = self.block_to_addr(src);
        let dst = self.block_to_addr(dst);

        for i in 0..HEAP_BLOCK_SIZE as isize {
            unsafe { *dst.offset(i) = *src.offset(i) }
        }
    }

    fn copy_blocks(&mut self, src: usize, dst: usize, count: usize) {
        for i in 0..count {
            self.copy_block(src + i, dst + i)
        }
    }

    fn align_block(val: usize) -> usize {
        if val % HEAP_BLOCK_SIZE as usize == 0 {
            return val;
        }

        val + val % HEAP_BLOCK_SIZE as usize
    }

    pub(super) fn alloc<T>(&mut self, size: usize) -> *mut T {
        self.alloc_blocks(Self::align_block(size)).cast()
    }

    pub(super) fn realloc(&mut self, old: Addr, size: usize) -> Addr {
        let count = Self::align_block(size);
        let new = self.alloc_blocks(count);
        self.copy_blocks(self.addr_to_block(new), self.addr_to_block(old), count);

        new
    }

    pub(super) fn free<T: ?Sized>(&mut self, ptr: *mut T) {
        let start_block = self.addr_to_block(ptr.cast());
        self.mark_blocks_free(start_block);
    }
}
