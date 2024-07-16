use crate::prelude::*;

use super::rawvec::RawVec;

use core::ops::{Deref, DerefMut};

pub struct Vec<T: Sized> {
    data: RawVec<T>,
    len: usize,
}

pub struct VecIter<'a, T> {
    vec: &'a Vec<T>,
    index: usize,
}

impl<'a, T> Iterator for VecIter<'a, T> {
    type Item = &'a T;
    fn next(&mut self) -> Option<Self::Item> {
        if self.index >= self.vec.len as usize {
            return None;
        }

        self.index += 1;
        Some(&self.vec[self.index - 1])
    }
}

impl<T: Copy> Vec<T> {
    pub fn new() -> Self {
        traceln!("Creating a new Vec");
        Self {
            data: RawVec::new(),
            len: 0,
        }
    }

    pub fn with_capacity(cap: usize) -> Self {
        traceln!("Creating Vec with {} capacity", cap);
        Self {
            data: RawVec::with_capacity(cap),
            len: 0,
        }
    }

    fn grow(&mut self) {
        self.data.grow();
    }

    pub fn push(&mut self, x: T) {
        if self.len == self.data.capacity() {
            self.grow()
        }

        unsafe {
            self.data.ptr().add(self.len).write(x);
            self.len += 1;
        }
    }

    pub fn pop(&mut self) -> Option<T> {
        if self.len == 0 {
            return None;
        }

        let x = unsafe { *self.data.ptr().add(self.len) };
        self.len -= 1;

        Some(x)
    }

    pub fn clear(&mut self) {
        self.len = 0;
    }

    #[inline]
    pub fn as_slice(&self) -> &[T] {
        self
    }

    #[inline]
    pub fn as_slice_mut(&mut self) -> &mut [T] {
        self
    }
}

use core::fmt::Debug;
impl<T: Debug> Debug for Vec<T> {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        writeln!(f, "[")?;
        for (i, el) in self.into_iter().enumerate() {
            write!(f, "\t{:#?}", el)?;

            if i != self.len as usize {
                writeln!(f, ",")?;
            }
        }
        writeln!(f, "]")?;

        Ok(())
    }
}

impl<T> Deref for Vec<T> {
    type Target = [T];
    fn deref(&self) -> &Self::Target {
        unsafe {
            core::ptr::slice_from_raw_parts(self.data.ptr(), self.data.capacity())
                .as_ref()
                .unwrap()
        }
    }
}

impl<T> DerefMut for Vec<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe {
            core::ptr::slice_from_raw_parts(self.data.ptr(), self.data.capacity())
                .cast_mut()
                .as_mut()
                .unwrap()
        }
    }
}

impl<T: Copy> Default for Vec<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Sized> Drop for Vec<T> {
    fn drop(&mut self) {
        traceln!("Dropping Vec");
        self.data.free()
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

impl<'a, T> IntoIterator for &'a Vec<T> {
    type Item = &'a T;
    type IntoIter = VecIter<'a, T>;
    fn into_iter(self) -> Self::IntoIter {
        VecIter {
            vec: self,
            index: 0,
        }
    }
}
