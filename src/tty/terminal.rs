use crate::prelude::*;

use core::fmt::{self, Write};

use super::TypeWriter;

const VGA_WIDTH: usize = 80;
const VGA_HEIGHT: usize = 25;

static mut TERMINAL: Global<Terminal> = Global::new(Terminal::new, "TERMINAL");

pub struct Terminal(TypeWriter);
impl Terminal {
    fn new() -> Self {
        Self(TypeWriter::new(0xB8000, VGA_WIDTH, VGA_HEIGHT))
    }
    pub fn init() {
        let mut terminal = unsafe { lock!(TERMINAL) };
        terminal.0.init()
    }
    pub fn print(args: fmt::Arguments) {
        let mut terminal = unsafe { lock!(TERMINAL) };
        terminal.0.write_fmt(args).unwrap()
    }
}

#[macro_export]
macro_rules! __print {
    ($($arg:tt)*) => {
        $crate::tty::terminal::Terminal::print(format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! __println {
    () => ($crate::print!("\n"));
    ($fmt:expr) => ($crate::__print!(concat!($fmt, "\n")));
    ($fmt:expr, $($arg:tt)*) => ($crate::__print!(
        concat!($fmt, "\n"), $($arg)*));
}
