extern "C" {
    fn _idt_init();
}

pub fn idt_init() {
    unsafe { _idt_init() }
}
