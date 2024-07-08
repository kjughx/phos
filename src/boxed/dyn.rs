use crate::prelude::*;

use core::{
    marker::Unsize,
    ops::{CoerceUnsized, Deref, DerefMut},
    ptr::Unique,
};

use super::KERNEL_HEAP as HEAP;

pub struct _Dyn<T: ?Sized>(Unique<T>);
impl<T> _Dyn<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = HEAP.lock().alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }

    pub fn drop(self) {
        trace!("Dropping Dyn");
        unsafe { HEAP.lock().free::<T>(self.0.as_ptr()) }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<_Dyn<U>> for _Dyn<T> {}

impl<T: ?Sized> Deref for _Dyn<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { self.0.as_ref() }
    }
}

impl<T: ?Sized> DerefMut for _Dyn<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { self.0.as_mut() }
    }
}
