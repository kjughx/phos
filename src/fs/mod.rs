use crate::disk::Stream;
use crate::prelude::*;

use crate::disk::{get_disk, Disk};
use crate::path::Path;

use core::any::Any;

mod filesystems;
use filesystems::fat16::Fat16;

pub const FILESYSTEM_COUNT: usize = 1;

#[derive(Debug)]
pub enum IOError {
    NotOurFS,
    FSNotFound,
    InvalidDisk,
    NoFS,
    NoSuchFile,
    NotAFile,
    InvalidArgument,
}

pub enum FileMode {
    ReadOnly,
}

pub enum SeekMode {
    StartOfFile,
    CurrentPosition,
    EndOfFile,
}

pub trait FileSystem {
    fn open(
        &self,
        stream: &mut dyn Stream,
        path: Path,
        mode: FileMode,
    ) -> Result<Box<dyn FileDescriptor>, IOError>;
    fn read(&self, fd: Box<dyn FileDescriptor>);
    fn seek(&self);
    fn stat(&self);
    fn close(&self);
    fn name(&self) -> &str;
    fn as_any(&self) -> &dyn Any;
}

pub trait FileDescriptor {
    fn read(&self, size: usize, count: usize, buf: &mut [u8]) -> Result<(), IOError>;
    fn write(&mut self, size: usize, count: usize, buf: &[u8]) -> Result<(), IOError>;
    fn seek(&mut self, offset: isize, whence: SeekMode);
}

pub fn resolve(disk: &mut Global<Disk>) -> Result<(), IOError> {
    if let Ok(fs) = Fat16::resolve(disk) {
        lock!(disk).register_filesystem(fs);
        return Ok(());
    }

    Err(IOError::FSNotFound)
}

pub fn open(path: Path, mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
    let Some(disk_id) = path.disk_id else {
        return Err(IOError::InvalidDisk);
    };

    let disk = lock!(get_disk(disk_id));

    let Some(ref fs) = disk.filesystem else {
        return Err(IOError::NoFS);
    };

    let mut stream = disk.stream();

    fs.open(&mut stream, path, mode)
}
