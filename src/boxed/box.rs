use crate::prelude::*;

use core::{
    marker::Unsize,
    ops::{CoerceUnsized, Deref, DerefMut, DispatchFromDyn},
    ptr::Unique,
};

use super::KERNEL_HEAP as HEAP;

#[doc(hidden)]
pub struct _Box<T: ?Sized>(Unique<T>);
impl<T> _Box<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = HEAP.lock().alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<_Box<U>> for _Box<T> {}
impl<T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<_Box<U>> for _Box<T> {}

impl<T: ?Sized> Drop for _Box<T> {
    fn drop(&mut self) {
        trace!("Droping box");
        unsafe { HEAP.lock().free::<T>(self.0.as_ptr()) }
    }
}

impl<T> Deref for _Box<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { core::mem::transmute::<Unique<T>, &T>(self.0) }
    }
}

impl<T> DerefMut for _Box<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { core::mem::transmute::<Unique<T>, &mut T>(self.0) }
    }
}
