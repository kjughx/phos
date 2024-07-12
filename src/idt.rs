extern "C" {
    fn _idt_init();
}

pub struct IDT;
impl IDT {
    pub fn init() {
        unsafe { _idt_init() }
    }
}

