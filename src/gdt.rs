extern "C" {
    fn _gdt_init();
}

pub struct Gdt;
impl Gdt {
    pub fn init() {
        unsafe { _gdt_init() }
    }
}
