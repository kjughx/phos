pub mod global;
pub mod mutex;

use core::{
    hint,
    sync::atomic::{AtomicBool, Ordering},
};

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
        // traceln!("Unlocking {}", self.id);
        self.locked.store(false, Ordering::Release);
    }

    fn id(&self) -> &'static str {
        self.id
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
