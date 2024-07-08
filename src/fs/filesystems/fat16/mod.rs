use crate::prelude::*;

mod r#impl;
mod private;
mod types;

use types::*;

use crate::{
    disk::{self, Disk, DiskStreamer},
    fs::{FileDescriptor, FileMode, FileSystem, IOError},
    path::Path,
};

use private::{FatDirectoryItem, FatH};
use r#impl::{FatDirectory, FatItem, FAT16_SIGNATURE};

pub struct Fat16 {
    disk_id: u32,
    header: FatH,
    root_dir: FatDirectory,
}

impl Fat16 {
    pub(self) fn new(disk_id: u32, root_dir: FatDirectory) -> Self {
        Self {
            disk_id,
            header: FatH::default(),
            root_dir,
        }
    }

    pub fn resolve(disk: &Global<Disk>) -> Result<Dyn<dyn FileSystem>, IOError> {
        let id = { disk.lock().id };
        let mut directory_stream = DiskStreamer::new(id);
        let header = FatH::new(id);
        trace!("{}", header.primary_header.sectors_per_cluster);
        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        let root_start = header.root();
        let size = header.primary_header.root_dir_entries as usize;
        let root_dir = FatDirectory::new(&mut directory_stream, root_start, size);

        Ok(Dyn::new(Self {
            disk_id: id,
            header,
            root_dir,
        }))
    }

    fn root(&self) -> &FatDirectory {
        &self.root_dir
    }

    fn get_directory_entry(&mut self, path: Path) -> Option<FatItem> {
        let Some(disk_id) = path.disk_id else {
            return None;
        };

        let mut iter = path.parts().into_iter();

        let root = self.root();
        let part = iter.next()?;

        let mut streamer = DiskStreamer::new(disk_id);

        let mut current = root.find(&mut streamer, part)?;

        for next in iter {
            match current {
                FatItem::Directory(ref dir) => current = dir.find(&mut streamer, next)?,
                FatItem::File(_) => return None,
            }
        }

        Some(current)
    }

    fn addr_of_data(&self) -> usize {
        self.header.root() + self.root_dir.size()
    }
}

impl FileSystem for Fat16 {
    fn open(&mut self, path: Path, _mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
        let Some(entry) = self.get_directory_entry(path) else {
            return Err(IOError::NoSuchFile);
        };

        let file = match entry {
            FatItem::Directory(_) => return Err(IOError::NotAFile),
            FatItem::File(f) => f,
        };

        let desc: Box<dyn FileDescriptor> = Box::new(FatFileDescriptor::new(self.disk_id, file));

        Ok(desc)
    }

    fn read(&self, _fd: Box<dyn FileDescriptor>) {
        todo!()
    }

    fn seek(&self) {
        todo!()
    }

    fn stat(&self) {
        todo!()
    }
    fn name(&self) -> &str {
        todo!()
    }

    fn close(&self) {
        todo!()
    }

    fn as_any(&self) -> &dyn core::any::Any {
        self
    }
}

pub struct FatFileDescriptor {
    item: FatDirectoryItem,
    disk_id: u32,
    pos: usize,
}

impl FatFileDescriptor {
    fn new(disk: u32, item: FatDirectoryItem) -> Self {
        Self {
            disk_id: disk,
            item,
            pos: 0,
        }
    }
}

impl FileDescriptor for FatFileDescriptor {
    fn read(&self, size: usize, count: usize, buf: &mut [u8]) -> Result<(), IOError> {
        if buf.len() < size * count {
            return Err(IOError::InvalidArgument);
        }

        let fs = match disk::get_disk(self.disk_id).lock().filesystem {
            None => return Err(IOError::NoFS),
            Some(ref fs) => fs
                .as_any()
                .downcast_ref::<Fat16>()
                .expect("whose filesystem is this...?"),
        };

        Ok(())
    }

    fn write(&mut self, size: usize, count: usize, buf: &[u8]) -> Result<(), IOError> {
        todo!()
    }

    fn seek(&mut self, offset: isize, whence: crate::fs::SeekMode) {
        todo!()
    }
}
