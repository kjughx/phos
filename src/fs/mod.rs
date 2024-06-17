use path::Path;
mod fs_impl;

use crate::disk::{get_disk, Disk};
use crate::sync::Global;
use crate::Box;
use fs_impl::fat16::Fat16;
pub mod path;

pub const FILESYSTEM_COUNT: usize = 1;

#[derive(Debug)]
pub enum IOError {
    NotOurFS,
    FSNotFound,
    InvalidDisk,
    NoFS,
}

pub enum FileMode {
    ReadOnly,
}

pub trait FileSystem {
    fn open(&self, path: Path, mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError>;
    fn read(&self, fd: Box<dyn FileDescriptor>);
    fn seek(&self);
    fn stat(&self);
    fn close(&self);
    fn name(&self) -> &str;
}

pub trait FileDescriptor {
    fn read(&self, size: usize, count: usize, buf: &mut [u8]);
    fn write(&mut self, size: usize, count: usize, buf: &[u8]);
}

pub fn resolve(disk: &mut Global<Disk>) -> Result<(), IOError> {
    if let Ok(fs) = Fat16::resolve(disk) {
        disk.lock().register_filesystem(fs);
        return Ok(());
    }

    Err(IOError::FSNotFound)
}

pub fn open(path: Path, mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
    let Some(disk_id) = path.disk_id else {
        return Err(IOError::InvalidDisk);
    };

    let disk = get_disk(disk_id);

    let Some(ref fs) = disk.lock().filesystem else {
        return Err(IOError::NoFS);
    };

    fs.open(path, mode)
}
