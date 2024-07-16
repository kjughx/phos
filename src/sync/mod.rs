pub mod global;
pub mod mutex;
pub mod shared;

use core::{
    hint,
    sync::atomic::{self, Ordering},
};

pub struct Lock {
    lock: atomic::AtomicUsize,
    id: Option<&'static str>, // For debugging,
}

impl Lock {
    pub const fn new(id: Option<&'static str>) -> Self {
        Self {
            lock: atomic::AtomicUsize::new(0),
            id,
        }
    }

    pub fn lock(&self) {
        lock(&self.lock, |a| a == 1, |_| 1);
    }

    pub fn is_locked(&self) -> bool {
        self.lock.load(Ordering::Acquire) == 1
    }

    pub fn unlock(&self) {
        match self
            .lock
            .compare_exchange(1, 0, Ordering::Acquire, Ordering::Relaxed)
        {
            Ok(_) => return,
            Err(_) => panic!("Poisoned lock: {:?}", self.id()),
        }
    }

    fn id(&self) -> Option<&'static str> {
        self.id
    }
}
//
// Pack read and write into the same value,
// usize::MAX means write-locked
// 0 means unlocked
// 1.. means read-locked and how many have it
pub struct RWLock {
    lock: atomic::AtomicUsize,
}

impl RWLock {
    pub fn new() -> Self {
        Self {
            lock: atomic::AtomicUsize::new(0),
        }
    }

    pub fn rlock(&self) {
        lock(&self.lock, |a| a == usize::MAX, |a| a + 1);
    }

    pub fn wlock(&self) {
        lock(&self.lock, |a| a != 0, |_| usize::MAX)
    }

    pub fn runlock(&self) {
        unlock(&self.lock, |a| a > 0, |a| a - 1)
    }
    pub fn wunlock(&self) {
        unlock(&self.lock, |a| a == usize::MAX, |_| 0)
    }
}

#[inline]
fn lock<F, B>(atomic: &atomic::AtomicUsize, is_locked: B, op: F)
where
    F: Fn(usize) -> usize,
    B: Fn(usize) -> bool,
{
    let mut current = atomic.load(atomic::Ordering::Acquire);
    loop {
        while is_locked(current) {
            hint::spin_loop();

            current = atomic.load(atomic::Ordering::Acquire);
            continue;
        }

        match atomic.compare_exchange_weak(
            current,
            op(current),
            Ordering::Acquire,
            Ordering::Relaxed,
        ) {
            Ok(_) => return,
            Err(old) => current = old,
        }
    }
}

#[inline]
fn unlock<F, B>(atomic: &atomic::AtomicUsize, is_locked: B, op: F)
where
    F: Fn(usize) -> usize,
    B: Fn(usize) -> bool,
{
    let mut current = atomic.load(atomic::Ordering::Acquire);
    loop {
        if !is_locked(current) {
            panic!("Poisoned lock: lock({})", current);
        }

        match atomic.compare_exchange_weak(
            current,
            op(current),
            Ordering::Acquire,
            Ordering::Relaxed,
        ) {
            Ok(_) => return,
            Err(old) => current = old,
        }
    }
}

#[macro_export]
macro_rules! lock {
    ($global:expr) => {
        $global.lock(file!(), line!())
    };
}

#[macro_export]
macro_rules! spinwhile {
    ($cond:expr) => {
        while $cond {}
    };
}

#[macro_export]
macro_rules! spinuntil {
    ($cond:expr) => {
        while !($cond) {}
    };
}
