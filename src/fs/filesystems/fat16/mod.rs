use crate::prelude::*;

mod r#impl;
mod private;
mod types;

use crate::{
    disk::{self, Disk, Stream},
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
        let (id, sector_size, header) = {
            let disk = lock!(disk);

            (disk.id, disk.sector_size, disk.stream().read_new::<FatH>())
        };

        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        let root_start = header.root() * sector_size;
        let size = header.primary_header.root_dir_entries as usize;
        let root_dir = {
            let mut stream = lock!(disk).stream();
            FatDirectory::new(&mut stream, root_start, size)
        };

        Ok(Dyn::new(Self {
            disk_id: id,
            header,
            root_dir,
        }))
    }

    fn root(&self) -> &FatDirectory {
        &self.root_dir
    }

    fn get_directory_entry(&self, stream: &mut dyn Stream, path: Path) -> Option<FatItem> {
        let mut iter = path.parts().into_iter();

        let root = self.root();
        let part = iter.next()?;

        {
            let mut current = root.find(stream, part)?;

            for next in iter {
                match current {
                    FatItem::Directory(ref dir) => current = dir.find(stream, next)?,
                    FatItem::File(_) => return None,
                }
            }
            Some(current)
        }
    }

    fn addr_of_data(&self) -> usize {
        self.header.root() + self.root_dir.size()
    }
}

impl FileSystem for Fat16 {
    fn open(
        &self,
        stream: &mut dyn Stream,
        path: Path,
        _mode: FileMode,
    ) -> Result<Box<dyn FileDescriptor>, IOError> {
        let Some(entry) = self.get_directory_entry(stream, path) else {
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

        let _ = match lock!(disk::get_disk(self.disk_id)).filesystem {
            None => return Err(IOError::NoFS),
            Some(ref fs) => fs
                .as_any()
                .downcast_ref::<Fat16>()
                .expect("whose filesystem is this...?"),
        };

        todo!();
    }

    fn write(&mut self, _size: usize, _count: usize, _buf: &[u8]) -> Result<(), IOError> {
        todo!()
    }

    fn seek(&mut self, _offset: isize, _whence: crate::fs::SeekMode) {
        todo!()
    }
}
