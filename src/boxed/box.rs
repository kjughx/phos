use crate::prelude::*;

use core::{
    marker::Unsize,
    ops::{CoerceUnsized, Deref, DerefMut},
    ptr::Unique,
};

use super::KERNEL_HEAP as HEAP;

pub struct _Box<T: ?Sized>(Unique<T>);
impl<T> _Box<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = lock!(HEAP).alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }
}

impl<T: ?Sized> Drop for _Box<T> {
    fn drop(&mut self) {
        unsafe { lock!(HEAP).free::<T>(self.0.as_ptr()) }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<_Box<U>> for _Box<T> {}

impl<T: ?Sized> Deref for _Box<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { self.0.as_ref() }
    }
}

impl<T: ?Sized> DerefMut for _Box<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { self.0.as_mut() }
    }
}
