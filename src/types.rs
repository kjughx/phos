use core::{
    cell::UnsafeCell,
    hint,
    ops::{Deref, DerefMut},
    sync::atomic::{AtomicBool, Ordering},
};
pub struct Addr(pub u32);

pub struct Lock {
    locked: AtomicBool,
    id: &'static str, // For debugging,
}

impl Lock {
    const fn new(id: &'static str) -> Self {
        Self {
            locked: AtomicBool::new(false),
            id,
        }
    }

    fn lock(&self) {
        while self.locked.load(Ordering::Acquire) {
            hint::spin_loop()
        }

        self.locked.store(true, Ordering::Release);
    }
    fn unlock(&self) {
        assert!(self.locked.load(Ordering::Acquire));
        self.locked.store(false, Ordering::Release);
    }
}

pub struct Mutex<T> {
    data: UnsafeCell<T>,
    lock: Lock,
}

impl<T> Mutex<T> {
    pub fn new(data: T, id: &'static str) -> Self {
        Self {
            data: UnsafeCell::new(data),
            lock: Lock::new(id),
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

enum _Global<T, F> {
    Uninit(F),
    Init(T),
    Poisoned,
}
pub struct Global<T, F = fn() -> T> {
    global: UnsafeCell<_Global<T, F>>,
    lock: Lock,
}

pub struct GlobalUnlocked<'a, T: 'a, F: FnOnce() -> T = fn() -> T> {
    locked: &'a Global<T, F>,
}

impl<'a, T: 'a, F: FnOnce() -> T> GlobalUnlocked<'a, T, F> {
    fn new(lock: &'a Global<T, F>) -> Self {
        Self { locked: lock }
    }

    pub fn unlock(&self) {
        self.locked.lock.unlock();
    }

    fn inner(&self) -> &T {
        let state = unsafe { &*self.locked.global.get() };
        match state {
            _Global::Init(t) => t,
            _Global::Uninit(_) => {
                Self::force_init(self);
                let _Global::Init(t) = (unsafe { &*self.locked.global.get() }) else {
                    unreachable!()
                };
                t
            }
            _Global::Poisoned => panic!(),
        }
    }

    fn inner_mut(&mut self) -> &mut T {
        let state = unsafe { &mut *self.locked.global.get() };
        match state {
            _Global::Init(t) => t,
            _Global::Uninit(_) => {
                Self::force_init(self);
                let _Global::Init(t) = (unsafe { &mut *self.locked.global.get() }) else {
                    unreachable!()
                };
                t
            }
            _Global::Poisoned => panic!(),
        }
    }

    fn force_init(this: &Self) {
        let state = unsafe { &mut *this.locked.global.get() };

        let _Global::Uninit(f) = core::mem::replace(state, _Global::Poisoned) else {
            unreachable!()
        };

        let data = f();

        unsafe { this.locked.global.get().write(_Global::Init(data)) };
    }
}

impl<T, F: FnOnce() -> T> Global<T, F> {
    pub const fn new(f: F, id: &'static str) -> Self {
        Self {
            global: UnsafeCell::new(_Global::Uninit(f)),
            lock: Lock::new(id),
        }
    }

    pub fn lock(&self) -> GlobalUnlocked<'_, T, F> {
        self.lock.lock();
        GlobalUnlocked::new(self)
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> Drop for GlobalUnlocked<'a, T, F> {
    fn drop(&mut self) {
        self.unlock()
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> Deref for GlobalUnlocked<'a, T, F> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        GlobalUnlocked::inner(self)
    }
}

impl<'a, T: 'a, F: FnOnce() -> T> DerefMut for GlobalUnlocked<'a, T, F> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        GlobalUnlocked::inner_mut(self)
    }
}

unsafe impl<T, F: FnOnce() -> T> Sync for Global<T, F> {}
