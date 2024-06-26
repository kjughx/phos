extern "C" {
    fn _gdt_init();
}

pub fn gdt_init() {
    unsafe { _gdt_init() }
}
