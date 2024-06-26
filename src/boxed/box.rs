use core::ops::CoerceUnsized;
use core::ops::Deref;
use core::{
    marker::Unsize,
    ops::{DerefMut, DispatchFromDyn},
    ptr::Unique,
};

use crate::trace;

use super::KERNEL_HEAP as HEAP;

pub struct Box<T: ?Sized>(Unique<T>);
impl<T> Box<T> {
    pub fn new(x: T) -> Self {
        unsafe {
            let t_ptr = HEAP.lock().alloc::<T>(core::mem::size_of::<T>());
            t_ptr.write(x);
            Self(Unique::new_unchecked(t_ptr))
        }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<Box<U>> for Box<T> {}
impl<T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<Box<U>> for Box<T> {}

impl<T: ?Sized> Drop for Box<T> {
    fn drop(&mut self) {
        trace!("Droping box");
        unsafe { HEAP.lock().free::<T>(self.0.as_ptr()) }
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
