use super::rawvec::RawVec;

pub struct Deque<T> {
    buf: RawVec<T>,
    head: usize,
    len: usize,
}

impl<T> Deque<T> {
    pub fn new() -> Self {
        Self {
            buf: RawVec::new(),
            head: 0,
            len: 0,
        }
    }

    #[inline]
    pub fn set_head(&mut self, new_head: usize) {
        self.head = new_head;
    }

    #[inline]
    pub fn head(&self) -> usize {
        self.head
    }

    #[inline]
    fn capacity(&self) -> usize {
        self.buf.capacity()
    }
    #[inline]
    fn ptr(&self) -> *mut T {
        self.buf.ptr()
    }

    #[inline]
    fn physical_idx(&self, idx: usize) -> usize {
        self.wrap_add(self.head, idx)
    }

    #[inline]
    fn wrap_add(&self, idx: usize, addend: usize) -> usize {
        wrap_index(idx.wrapping_add(addend), self.capacity())
    }

    #[inline]
    fn wrap_sub(&self, idx: usize, subtrahend: usize) -> usize {
        wrap_index(
            idx.wrapping_sub(subtrahend).wrapping_add(self.capacity()),
            self.capacity(),
        )
    }

    #[inline]
    fn is_full(&self) -> bool {
        self.len == self.capacity()
    }

    #[inline]
    unsafe fn copy_nonoverlapping(&mut self, src: usize, dst: usize, len: usize) {
        unsafe {
            core::ptr::copy_nonoverlapping(self.ptr().add(src), self.ptr().add(dst), len);
        }
    }

    #[inline]
    unsafe fn copy(&mut self, src: usize, dst: usize, len: usize) {
        unsafe {
            core::ptr::copy(self.ptr().add(src), self.ptr().add(dst), len);
        }
    }

    #[inline]
    fn write(&mut self, offset: usize, val: T) {
        unsafe { self.ptr().add(offset).write(val) }
    }

    fn grow(&mut self) {
        // We increase the underlying buffer but now the deque is not circular anymore
        let old_capacity = self.capacity();
        self.buf.grow();
        let new_capacity = self.capacity();

        // Straight out of rust alloc/collections/vec_deque/mod.rs: handle_capacity_increase
        //
        // Move the shortest contiguous section of the ring buffer
        //
        // H := head
        // L := last element (`self.to_physical_idx(self.len - 1)`)
        //
        //    H             L
        //   [o o o o o o o o ]
        //    H             L
        // A [o o o o o o o o . . . . . . . . ]
        //        L H
        //   [o o o o o o o o ]
        //          H             L
        // B [. . . o o o o o o o o . . . . . ]
        //              L H
        //   [o o o o o o o o ]
        //              L                 H
        // C [o o o o o o . . . . . . . . o o ]

        // If head <= tail
        if self.head <= old_capacity - self.len {
            return; // no need to do anything
        }

        let head_len = old_capacity - self.head;
        let tail_len = self.len - head_len;
        if head_len > tail_len && new_capacity - old_capacity >= tail_len {
            // B
            unsafe {
                self.copy_nonoverlapping(0, old_capacity, tail_len);
            }
        } else {
            // C
            let new_head = new_capacity - head_len;
            unsafe {
                // can't use copy_nonoverlapping here, because if e.g. head_len = 2
                // and new_capacity = old_capacity + 1, then the heads overlap.
                self.copy(self.head, new_head, head_len);
            }
            self.head = new_head;
        }
    }

    pub fn push_back(&mut self, t: T) -> usize {
        if self.is_full() {
            self.grow()
        }

        let idx = self.physical_idx(self.len);
        self.write(idx, t);
        self.len += 1;

        idx
    }

    pub fn push_front(&mut self, t: T) -> usize {
        if self.is_full() {
            self.grow();
        }

        let idx = self.wrap_sub(self.head, 1);
        self.write(idx, t);
        self.len += 1;

        idx
    }
}

#[inline]
fn wrap_index(logical_index: usize, capacity: usize) -> usize {
    debug_assert!(
        (logical_index == 0 && capacity == 0)
            || logical_index < capacity
            || (logical_index - capacity) < capacity
    );
    if logical_index >= capacity {
        logical_index - capacity
    } else {
        logical_index
    }
}
