extern "C" {
    fn _paging_load_directory(directory: *const usize);
}

pub fn load_directory(directory: *const usize) {
    unsafe { _paging_load_directory(directory) }
}
