#![no_std] // don't link the Rust standard library
#![no_main] // disable all Rust-level entry points
#![feature(asm)]
#![feature(naked_functions)]

#[no_mangle]
pub static DATA_SEG: u32 = 0x10;

#[no_mangle]
#[naked]
#[link_section = ".start"]
extern "C" fn _start() -> ! {
    unsafe {
        asm!(
            ".code32",
            "mov ax, DATA_SEG",
            "mov ds, ax",
            "mov es, ax",
            "mov fs, ax",
            "mov gs, ax",
            "mov ss, ax",
            "mov ebp, 0x00200000",
            "mov esp, ebp",
            "in al, 0x92",
            "or al, 2",
            "out 0x92, al",
            "call kernel_main",
            "loop:",
            "jmp loop",
            options(noreturn)
        );
    }
}

use core::panic::PanicInfo;

#[inline(never)]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

static LOCKED_WRITE: u8 = 0b01;
static LOCKED_READ: u8 = 0b10;
pub struct Mutex<T> {
    target: T,
    locked: u8,
}

impl<T> Mutex<T> {
    pub const fn new(value: T) -> Self {
        Self {
            target: value,
            locked: 0,
        }
    }

    pub fn lock(&mut self) -> &T {
        while self.locked != LOCKED_WRITE {}
        self.locked = LOCKED_READ;
        return &self.target;
    }
    pub fn lock_mut(&mut self) -> &mut T {
        while self.locked != 0 {}
        self.locked = LOCKED_WRITE;
        return &mut self.target;
    }

    pub fn free(&mut self) {
        self.locked = 0; // Set free to true
    }
}

impl<T> Drop for Mutex<T> {
    fn drop(&mut self) {
        assert_ne!(self.locked, 0); // This is a bug
        self.locked = 0;
    }
}

const VGA_WIDTH: isize = 80;
const VGA_HEIGHT: isize = 25;

static mut TTY: Tty = Tty {
    base: 0xB8000,
    width: VGA_WIDTH,
    height: VGA_HEIGHT,
    ix: 0,
    iy: 0,
};

struct Tty {
    base: u32,
    width: isize,
    height: isize,
    ix: isize,
    iy: isize,
}

static COLOR_WHITE: u8 = 15;
impl Tty {
    pub fn clear(&mut self) {
        for y in 0..=self.height {
            for x in 0..=self.width {
                unsafe {
                    *(self.base as *mut u16).offset(y * self.width + x) = 0;
                }
            }
        }
    }

    fn make_char(c: char, color: u8) -> u16 {
        return (color as u16) << 8 | (c as u16);
    }

    fn backspace(&mut self) {
        if self.ix == 0 && self.iy == 0 {
            return;
        }

        if self.ix == 0 {
            self.iy -= 1;
            self.ix = self.width;
        }

        self.ix -= 1;
        self.write_char(self.ix, self.iy, ' ', COLOR_WHITE);
        self.ix -= 1;
    }

    pub fn write_char(&mut self, ix: isize, iy: isize, c: char, color: u8) {
        unsafe {
            *(self.base as *mut u16).offset(iy * self.width + ix) = Self::make_char(c, color);
        }
    }
}

pub fn clear() {
    unsafe { TTY.clear() }
}

pub fn print(msg: &str) {
    for (i, c) in msg.chars().enumerate() {
        unsafe { TTY.write_char(i as isize, 0, c, COLOR_WHITE) };
    }
}

#[no_mangle] // don't mangle the name of this function
pub extern "C" fn kernel_main() -> ! {
    clear();
    print("Hello, World!");
    loop {}
}
