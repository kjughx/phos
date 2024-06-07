use crate::disk::Disk;
mod fat_private;
pub mod fat16;

#[derive(Debug)]
pub enum IOError {
    NotOurFS,
}

pub trait FileSystem {
    fn resolve(&self, disk: &Disk) -> Result<(), IOError>;
    fn open(&self);
    fn read(&self);
    fn seek(&self);
    fn stat(&self);
    fn close(&self);
    fn name(&self) -> &str;
}

pub fn resolve() {
    fat16::resolve();
}
