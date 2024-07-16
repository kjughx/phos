use crate::prelude::*;

use crate::sync::RWLock;

use core::ptr::NonNull;
use core::sync::atomic;

// This struct owns the value
struct SharedInner<T: Sized> {
    strong: atomic::AtomicUsize,
    weak: atomic::AtomicUsize,
    rwlock: RWLock,
    data: T,
}

impl<T> SharedInner<T> {
    fn new(t: T) -> Self {
        Self {
            strong: atomic::AtomicUsize::new(1),
            weak: atomic::AtomicUsize::new(1),
            rwlock: RWLock::new(),
            data: t,
        }
    }
}

unsafe impl<T: Sized + Sync + Send> Send for SharedInner<T> {}
unsafe impl<T: Sized + Sync + Send> Sync for SharedInner<T> {}

// An atomically reference-counted mutually exclusive accessed pointer to
// a value
pub struct Shared<T>(NonNull<SharedInner<T>>);

impl<T> Shared<T> {
    pub fn new(t: T) -> Self {
        let inner = Dyn::new(SharedInner::new(t)).as_ptr();
        Self(inner)
    }

    fn free(&mut self) {
        let inner = unsafe { Dyn::from_ptr(self.0.as_ptr()) };
        inner.drop()
    }

    fn inner(&self) -> &SharedInner<T> {
        unsafe { self.0.as_ref() }
    }
    fn inner_mut(&mut self) -> &mut SharedInner<T> {
        unsafe { self.0.as_mut() }
    }

    fn with_wlock<F>(&mut self, f: F)
    where
        F: FnOnce(&mut SharedInner<T>),
    {
        let inner = self.inner_mut();
        inner.rwlock.wlock();
        f(inner);
        inner.rwlock.wunlock()
    }

    #[inline]
    fn from_inner(ptr: NonNull<SharedInner<T>>) -> Self {
        Self(ptr)
    }
}

impl<T> Clone for Shared<T> {
    fn clone(&self) -> Self {
        let inner = self.inner();
        inner.strong.fetch_add(1, atomic::Ordering::Relaxed);
        Self::from_inner(self.0)
    }
}

pub struct Weak<T>(NonNull<SharedInner<T>>);

impl<T> Drop for Weak<T> {
    fn drop(&mut self) {
        let weak = unsafe { &(*self.0.as_ptr()).weak };

        // Last reference to value
        if weak.fetch_sub(1, atomic::Ordering::Release) == 1 {}
    }
}
