use crate::types::Global;
use core::{
    ops::{Deref, DerefMut, Index, IndexMut},
    ptr::Unique,
};

mod heap;
use heap::{Heap, HEAP_BLOCK_SIZE};

const KERNEL_HEAP_SIZE: usize = 100 * 1024 * 1024; // 100MB

static mut KERNEL_HEAP: Global<Heap> = Global::new(
    || {
        Heap::new(
            0x00007E00,
            KERNEL_HEAP_SIZE / HEAP_BLOCK_SIZE as usize,
            0x01000000,
        )
    },
    "KERNEL_HEAP",
);

pub struct Dyn<T: ?Sized>(Unique<T>);
impl<T> Dyn<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = KERNEL_HEAP.lock().alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }

    pub fn drop(self) {
        unsafe { KERNEL_HEAP.lock().free::<T>(self.0.as_ptr()) }
    }
}

impl<T> Deref for Dyn<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { self.0.as_ref() }
    }
}

impl<T> DerefMut for Dyn<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { self.0.as_mut() }
    }
}

pub struct Box<T: ?Sized>(Unique<T>);
impl<T> Box<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = KERNEL_HEAP.lock().alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }
}

impl<T: ?Sized> Drop for Box<T> {
    fn drop(&mut self) {
        unsafe { KERNEL_HEAP.lock().free::<T>(self.0.as_ptr()) }
    }
}

impl<T> Deref for Box<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { core::mem::transmute::<Unique<T>, &T>(self.0) }
    }
}

impl<T> DerefMut for Box<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { core::mem::transmute::<Unique<T>, &mut T>(self.0) }
    }
}

#[derive(Clone)]
pub struct Vec<T: Sized> {
    data: Unique<T>,
    cap: usize,
    len: isize,
}

const DEFAULT_VEC_CAP: usize = 16;

impl<T: Copy> Vec<T> {
    pub fn new() -> Self {
        unsafe {
            let t_ptr = core::mem::transmute::<*mut u8, *mut T>(
                KERNEL_HEAP
                    .lock()
                    .alloc(DEFAULT_VEC_CAP * core::mem::size_of::<T>()),
            );

            Self {
                data: Unique::new_unchecked(t_ptr),
                cap: DEFAULT_VEC_CAP,
                len: 0,
            }
        }
    }

    pub fn with_capacity(cap: usize) -> Self {
        unsafe {
            let t_ptr = core::mem::transmute::<*mut u8, *mut T>(
                KERNEL_HEAP.lock().alloc(cap * core::mem::size_of::<T>()),
            );

            Self {
                data: Unique::new_unchecked(t_ptr),
                cap,
                len: 0,
            }
        }
    }

    fn grow(&mut self) {
        unsafe {
            self.data = Unique::new_unchecked(core::mem::transmute::<*mut u8, *mut T>(
                KERNEL_HEAP
                    .lock()
                    .realloc(self.data.as_ptr() as *mut u8, 2 * self.cap),
            ));
        }
        self.cap *= 2;
    }

    pub fn push(&mut self, x: T) {
        if self.len as usize == self.cap {
            self.grow()
        }

        unsafe {
            *self.data.as_ptr().offset(self.len + 1) = x;
        }
        self.len += 1;
    }

    pub fn pop(&mut self) -> Option<T> {
        if self.len == 0 {
            return None;
        }

        let x = unsafe { *self.data.as_ptr().offset(self.len) };
        self.len -= 1;

        Some(x)
    }

    pub fn clear(&mut self) {
        self.len = 0;
    }
}
impl<T: Copy> Default for Vec<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Sized> Drop for Vec<T> {
    fn drop(&mut self) {
        unsafe { KERNEL_HEAP.lock().free(self.data.as_ptr()) }
    }
}
impl<T> Index<isize> for Vec<T> {
    type Output = T;
    fn index(&self, index: isize) -> &Self::Output {
        let index = if index < 0 {
            self.len - index - 1
        } else {
            index
        };

        unsafe { self.data.as_ptr().offset(index).as_ref().unwrap() }
    }
}

impl<T> IndexMut<isize> for Vec<T> {
    fn index_mut(&mut self, index: isize) -> &mut Self::Output {
        let index = if index < 0 {
            self.len - index - 1
        } else {
            index
        };
        unsafe { self.data.as_ptr().offset(index).as_mut().unwrap() }
    }
}

impl<T: Copy> FromIterator<T> for Vec<T> {
    fn from_iter<I: IntoIterator<Item = T>>(iter: I) -> Self {
        let mut vec = Vec::new();
        for element in iter {
            vec.push(element);
        }

        vec
    }
}

pub struct DynArray<T>(Vec<T>);
impl<T: Copy> DynArray<T> {
    pub fn new(cap: usize) -> Self {
        Self(Vec::with_capacity(cap))
    }
}

impl<T> Index<isize> for DynArray<T> {
    type Output = T;
    fn index(&self, index: isize) -> &Self::Output {
        self.0.index(index)
    }
}

impl<T> IndexMut<isize> for DynArray<T> {
    fn index_mut(&mut self, index: isize) -> &mut Self::Output {
        self.0.index_mut(index)
    }
}
