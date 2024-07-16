use crate::prelude::*;

use core::{
    cell::UnsafeCell,
    ops::{Deref, DerefMut},
};

use super::Lock;

enum State<T, F> {
    Uninit(F),
    Init(T),
    Poisoned,
}
pub struct _Global<T, F = fn() -> T> {
    data: UnsafeCell<State<T, F>>,
    lock: Lock,
}

unsafe impl<T, F: FnOnce() -> T> Sync for _Global<T, F> {}

impl<T, F: FnOnce() -> T> _Global<T, F> {
    pub const fn new(f: F, id: &'static str) -> Self {
        Self {
            data: UnsafeCell::new(State::Uninit(f)),
            lock: Lock::new(Some(id)),
        }
    }

    pub fn lock(&self, file: &'static str, line: u32) -> GlobalInner<'_, T, F> {
        self.lock.lock();
        __trace!("[{}:{}] Locking {}\n", file, line, self.id());
        GlobalInner::new(self)
    }

    pub fn id(&self) -> &'static str {
        match self.lock.id {
            Some(id) => id,
            None => "UN-NAMED",
        }
    }
}

pub struct GlobalInner<'a, T: 'a, F: FnOnce() -> T = fn() -> T> {
    global: &'a _Global<T, F>,
}

impl<'a, T: 'a, F: FnOnce() -> T> GlobalInner<'a, T, F> {
    fn new(lock: &'a _Global<T, F>) -> Self {
        Self { global: lock }
    }

    pub fn inner(&self) -> &T {
        let state = unsafe { &*self.global.data.get() };

        match state {
            State::Init(data) => data,
            State::Uninit(_) => {
                Self::force_init(self);
                let State::Init(t) = (unsafe { &*self.global.data.get() }) else {
                    unreachable!("State should be Init after forcing init")
                };
                t
            }
            State::Poisoned => panic!("Poisoned Global"),
        }
    }

    pub fn inner_mut(&mut self) -> &mut T {
        let state = unsafe { &mut *self.global.data.get() };
        match state {
            State::Init(t) => t,
            State::Uninit(_) => {
                Self::force_init(self);
                let State::Init(t) = (unsafe { &mut *self.global.data.get() }) else {
                    unreachable!()
                };
                t
            }
            State::Poisoned => panic!(),
        }
    }

    fn force_init(this: &Self) {
        let state = unsafe { &mut *this.global.data.get() };

        let State::Uninit(f) = core::mem::replace(state, State::Poisoned) else {
            unreachable!()
        };

        let data = f();

        unsafe { this.global.data.get().write(State::Init(data)) };
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> Drop for GlobalInner<'a, T, F> {
    fn drop(&mut self) {
        self.global.lock.unlock();
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> Deref for GlobalInner<'a, T, F> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        GlobalInner::inner(self)
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> DerefMut for GlobalInner<'a, T, F> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        GlobalInner::inner_mut(self)
    }
}
