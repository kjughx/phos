use core::{
    marker::Unsize,
    ops::{CoerceUnsized, Deref, DerefMut},
    ptr::Unique,
};

use crate::trace;

use super::KERNEL_HEAP as HEAP;

pub struct Dyn<T: ?Sized>(Unique<T>);
impl<T> Dyn<T> {
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

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<Dyn<U>> for Dyn<T> {}

impl<T: ?Sized> Deref for Dyn<T> {
    type Target = T;
    fn deref(&self) -> &T {
        unsafe { self.0.as_ref() }
    }
}

impl<T: ?Sized> DerefMut for Dyn<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { self.0.as_mut() }
    }
}
