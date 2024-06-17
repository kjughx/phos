use core::fmt::{self, Write};

use crate::io::{insb, outb};

const SERIAL_PORT: u16 = 0x3F8;

pub struct SerialPort;

impl SerialPort {
    #[allow(clippy::new_without_default)]
    pub fn new() -> SerialPort {
        SerialPort
    }

    fn init(&self) {
        // Configure the baud rate
        outb(SERIAL_PORT + 1, 0x00);
        outb(SERIAL_PORT + 3, 0x80);
        outb(SERIAL_PORT, 0x03);
        outb(SERIAL_PORT + 1, 0x00);
        outb(SERIAL_PORT + 3, 0x03);
        outb(SERIAL_PORT + 2, 0xC7);
        outb(SERIAL_PORT + 4, 0x0B);
    }

    fn is_transmit_empty(&self) -> bool {
        insb(SERIAL_PORT + 5) & 0x20 != 0
    }
}

impl Write for SerialPort {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for byte in s.bytes() {
            self.write_byte(byte);
        }
        Ok(())
    }
}

impl SerialPort {
    fn write_byte(&self, byte: u8) {
        while !self.is_transmit_empty() {}
        outb(SERIAL_PORT, byte)
    }
}

#[macro_export]
macro_rules! _trace {
    ($($arg:tt)*) => {
        $crate::serial::_print(format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! trace {
    () => ($crate::_trace!("\n"));
    ($fmt:expr) => ($crate::_trace!(concat!("[{}:{}] ", $fmt, file!(), line!(), "\n")));
    ($fmt:expr, $($arg:tt)*) => ($crate::_trace!(
        concat!("[{}:{}] ", $fmt, "\n"), file!(), line!(), $($arg)*));
}

#[doc(hidden)]
pub fn _print(args: fmt::Arguments) {
    use core::fmt::Write;
    let mut serial_port = SerialPort::new();
    serial_port.init();
    serial_port.write_fmt(args).unwrap();
}
