mod fat_impl;
mod fat_private;

use crate::{disk::{Disk, DiskStreamer}, fs::{FileDescriptor, FileMode, FileSystem, IOError}, path::Path, sync::Global, Box, Dyn};

use fat_impl::{FatDirectory, FatItem, FAT16_SIGNATURE};
use fat_private::FatH;

pub struct Fat16 {
    header: FatH,
    root_dir: FatDirectory,
    cluster_stream: DiskStreamer,
    fat_stream: DiskStreamer,
    directory_stream: DiskStreamer,
}

impl Fat16 {
    pub(self) fn new(disk_id: usize, root_dir: FatDirectory) -> Self {
        Self {
            header: FatH::default(),
            root_dir,
            cluster_stream: DiskStreamer::new(disk_id),
            fat_stream: DiskStreamer::new(disk_id),
            directory_stream: DiskStreamer::new(disk_id),
        }
    }

    pub fn resolve(disk: &Global<Disk>) -> Result<Dyn<dyn FileSystem>, IOError> {
        let id = { disk.lock().id };
        let mut directory_stream = DiskStreamer::new(id);
        let header = FatH::new(id);
        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        let root_start = header.root();
        let size = header.primary_header.root_dir_entries as usize;
        let root_dir = FatDirectory::new(&mut directory_stream, root_start, size);

        Ok(Dyn::new(Self {
            header,
            root_dir,
            directory_stream,
            cluster_stream: DiskStreamer::new(id),
            fat_stream: DiskStreamer::new(id),
        }))
    }

    fn root(&self) -> FatDirectory {
        // TODO: Don't want to have to clone it
        self.root_dir.clone()
    }

    fn get_directory_entry(&mut self, path: Path) -> Option<FatItem> {
        let mut iter = path.parts().into_iter();

        let root = self.root();
        let part = iter.next()?;

        let mut current = root.find(&mut self.cluster_stream, part)?;

        for next in iter {
            match current {
                FatItem::Directory(ref dir) => {
                    current = dir.find(&mut self.cluster_stream, next)?
                }
                FatItem::File(_) => return None,
            }
        }

        Some(current)
    }
}

impl FileSystem for Fat16 {
    fn open(&mut self, path: Path, _mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
        let Some(entry) = self.get_directory_entry(path) else {
            return Err(IOError::NoSuchFile);
        };

        let desc: Box<dyn FileDescriptor> = Box::new(FatFileDescriptor::new(entry));

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
}

pub struct FatFileDescriptor {
    item: FatItem,
    pos: u32,
}

impl FatFileDescriptor {
    fn new(item: FatItem) -> Self {
        Self { item, pos: 0 }
    }
}

impl FileDescriptor for FatFileDescriptor {
    fn read(&self, size: usize, count: usize, buf: &mut [u8]) {
        todo!()
    }

    fn write(&mut self, size: usize, count: usize, buf: &[u8]) {}
}
