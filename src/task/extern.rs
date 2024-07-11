extern "C" {
    fn _tss_load(segment: usize);
}

pub struct Tss;
impl Tss {
    pub fn load(segment: usize) {
        unsafe { _tss_load(segment) }
    }
}
