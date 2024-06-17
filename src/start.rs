#[no_mangle]
pub static DATA_SEG: u32 = 0x10;

use core::arch::asm;

use crate::trace;

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
            "mov al, 00010001b",
            "out 0x20, al",
            "mov al, 0x20",
            "out 0x21, al",
            "mov al, 00000001b",
            "out 0x21, al",
            "call kernel_main",
            options(noreturn)
        );
    }
}

#[inline(never)]
#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    let loc = info.location().unwrap(); // TODO: This might not always return Some(&Location)

    trace!("Kernel panic: [{}:{}]", loc.file(), loc.line());

    if let Some(msg) = info.payload().downcast_ref::<&str>() {
        trace!("{}", msg);
    } else if let Some(msg) = info.message() {
        trace!("{}", msg);
    } else {
        trace!("{}", "Kernel Panic");
    }
    unsafe { asm!("hlt", options(noreturn)) }
}
