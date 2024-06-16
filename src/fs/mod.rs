use crate::disk::Disk;
use crate::fs_impl::fat16;
use crate::Box;
pub mod path;

pub const FILESYSTEM_COUNT: usize = 1;

#[derive(Debug)]
pub enum IOError {
    NotOurFS,
    FSNotFound,
}

pub enum FileMode {
    ReadOnly,
}

pub trait FileSystem {
    fn resolve(&mut self, disk: &Disk) -> Result<&dyn FileSystem, IOError>;
    fn open(&self, mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError>;
    fn read(&self, fd: Box<dyn FileDescriptor>);
    fn seek(&self);
    fn stat(&self);
    fn close(&self);
    fn name(&self) -> &str;
}

pub trait FileDescriptor {
    fn read(&self, size: usize, count: usize, out: &mut [u8]);
}

pub fn resolve(disk: &mut Disk) -> Result<(), IOError> {
    if let Ok(fs) = fat16::resolve(disk) {
        disk.lock().register_filesystem(fs);
        return Ok(());
    }

    Err(IOError::FSNotFound)
}
