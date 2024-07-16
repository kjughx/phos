use crate::{alloc, free, realloc};

use core::ptr::Unique;

/// Always capacity % 2 == 0
pub struct RawVec<T> {
    ptr: Unique<T>,
    cap: usize,
}

const DEFAULT_CAPACITY: usize = 16;

impl<T> RawVec<T> {
    pub fn new() -> Self {
        Self::with_capacity(DEFAULT_CAPACITY)
    }

    pub fn with_capacity(cap: usize) -> Self {
        let data = unsafe {
            let ptr =
                core::mem::transmute::<*mut u8, *mut T>(alloc(cap * core::mem::size_of::<T>()));
            Unique::new_unchecked(ptr)
        };

        Self { ptr: data, cap }
    }

    pub fn ptr(&self) -> *mut T {
        self.ptr.as_ptr()
    }

    pub fn grow(&mut self) {
        unsafe {
            self.ptr = Unique::new_unchecked(core::mem::transmute::<*mut u8, *mut T>(realloc(
                self.ptr.as_ptr() as *mut u8,
                2 * self.cap,
            )));
        }
        self.cap *= 2;
    }

    pub fn free(&mut self) {
        free(self.ptr.as_ptr());
    }

    pub fn capacity(&self) -> usize {
        self.cap
    }
}
