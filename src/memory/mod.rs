use core::{lazy::Lazy, ops::{Deref, DerefMut}, ptr::{self, Unique}};
use crate::types::Mutex;

const HEAP_BLOCK_SIZE: u32 = 4096;
const BLOCK_FREE: u8 =     1 << 0;
const BLOCK_TAKEN: u8 =    1 << 1;
const BLOCK_FIRST: u8 =    1 << 4;
const BLOCK_HAS_NEXT: u8 = 1 << 5;

const KERNEL_HEAP_SIZE: usize = 100 * 1024 * 1024; // 100MB

#[derive(Debug)]
enum MemoryError {
    NoAction,
    InvalidAlignment,
    OutOfMemory,
    Other,
}

type Addr = *mut u8;

struct Heap {
    entries: Unique<[u8]>,
    count: usize,
    start: Addr,
}

impl Heap {
    pub fn new(entries: u32, count: usize, start: u32) -> Self {
        if count == 0 {
            panic!();
        }

        if start %  HEAP_BLOCK_SIZE != 0 {
            panic!();
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
        return (self.start as u32 + block as u32 * HEAP_BLOCK_SIZE) as Addr;
    }

    fn addr_to_block(&self, addr: Addr) -> usize {
        assert!(addr > self.start);
        return (addr as usize - self.start as usize) / (HEAP_BLOCK_SIZE as usize);
    }

    fn mark_blocks_taken(&mut self, start_block: usize, total_blocks: usize) {
        if total_blocks == 1 {
            unsafe {
                self.entries.as_mut()[start_block] = BLOCK_TAKEN | BLOCK_FIRST;
            }
            return
        }

        unsafe {
            self.entries.as_mut()[start_block] = BLOCK_TAKEN | BLOCK_FIRST | BLOCK_HAS_NEXT;
            ptr::write_bytes((self.entries.as_ptr() as *mut u8).offset(start_block as isize + 1), BLOCK_TAKEN | BLOCK_HAS_NEXT, total_blocks - 2);
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
        unsafe {entries.as_ref()[offset] & 0x0f}
    }

    fn get_free_block(&self, count: usize) -> Result<usize, MemoryError>{
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

        return Ok(bs as usize);
    }

    fn alloc_blocks(&mut self, block_count: usize) -> *mut u8 {
        let s = self.get_free_block(block_count).unwrap();

        let addr = self.block_to_addr(s);
        self.mark_blocks_taken(s, block_count);

        return addr;
    }

    fn align_block(val: usize) -> usize {
        if val % HEAP_BLOCK_SIZE as usize == 0 {
            return val;
        }

        return val + val % HEAP_BLOCK_SIZE as usize;
    }

    fn alloc(&mut self, bytes: usize) -> Addr {
        self.alloc_blocks(Self::align_block(bytes))
    }

    fn free(&mut self, ptr: Addr) {
        self.mark_blocks_free(self.addr_to_block(ptr));
    }
}

static mut KERNEL_HEAP: Lazy<Mutex<Heap>> = Lazy::new(||
    Mutex::new(Heap::new(0x00007E00, KERNEL_HEAP_SIZE / HEAP_BLOCK_SIZE as usize, 0x01000000))
);

pub struct Dyn<T: ?Sized>(Unique<T>);
impl<T> Dyn<T> {
    pub fn new(x: T) -> Self {
        let mut heap = unsafe { KERNEL_HEAP.lock() };
        let ptr = heap.alloc(core::mem::size_of::<T>());
        unsafe {
            let t_ptr = core::mem::transmute::<*mut u8, *mut T>(ptr);
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }

    pub fn drop(self) {
        unsafe {
            KERNEL_HEAP.lock().free(self.0.as_ptr() as Addr)
        }
    }
}

impl<T> Deref for Dyn<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe{self.0.as_ref()}
    }
}

impl<T> DerefMut for Dyn<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe{self.0.as_mut()}
    }
}

pub struct Box<T: ?Sized>(Unique<T>);
impl<T> Box<T> {
    pub fn new(x: T) -> Self {
        let mut heap = unsafe { KERNEL_HEAP.lock() };
        let ptr = heap.alloc(core::mem::size_of::<T>());
        unsafe {
            let t_ptr = core::mem::transmute::<*mut u8, *mut T>(ptr);
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }
}

impl<T: ?Sized> Drop for Box<T> {
    fn drop(&mut self) {
        unsafe {
            KERNEL_HEAP.lock().free(self.0.as_ptr() as *mut u8)
        }
    }
}

impl<T> Deref for Box<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe{core::mem::transmute::<Unique<T>, &T>(self.0)}
    }
}

impl<T> DerefMut for Box<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe{core::mem::transmute::<Unique<T>, &mut T>(self.0)}
    }
}

