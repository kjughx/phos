extern "C" {
    fn _idt_init();
}

pub struct Idt;
impl Idt {
    pub fn init() {
        unsafe { _idt_init() }
    }
}
