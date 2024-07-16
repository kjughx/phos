use super::Lock;

use core::{
    cell::UnsafeCell,
    ops::{Deref, DerefMut},
};

pub struct Mutex<T> {
    data: UnsafeCell<T>,
    lock: Lock,
}

impl<T> Mutex<T> {
    pub fn new(data: T) -> Self {
        Self {
            data: UnsafeCell::new(data),
            lock: Lock::new(None),
        }
    }

    pub fn lock(&self) -> MutexGuard<'_, T> {
        self.lock.lock();
        MutexGuard::new(self)
    }
}

unsafe impl<T> Sync for Mutex<T> {}

pub struct MutexGuard<'a, T: 'a> {
    locked: &'a Mutex<T>,
}

impl<'a, T: 'a> MutexGuard<'a, T> {
    fn new(lock: &'a Mutex<T>) -> Self {
        Self { locked: lock }
    }

    pub fn unlock(&self) {
        self.locked.lock.unlock();
    }
}

impl<T> Drop for MutexGuard<'_, T> {
    fn drop(&mut self) {
        self.unlock();
    }
}

impl<T> Deref for MutexGuard<'_, T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        unsafe { &*self.locked.data.get() }
    }
}

impl<T> DerefMut for MutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { &mut *self.locked.data.get() }
    }
}
