mod fat_impl;
mod fat_private;

use crate::{
    disk::{Disk, DiskStreamer, SECTOR_SIZE},
    fs::{path::Path, FileDescriptor, FileMode, FileSystem, IOError},
    sync::Global,
    Box, Dyn,
};
use core::mem;

use fat_impl::{FatDirectory, FAT16_SIGNATURE};
use fat_private::FatH;

pub struct Fat16 {
    header: FatH,
    root_dir: mem::MaybeUninit<FatDirectory>,
    cluster_stream: DiskStreamer,
    fat_stream: DiskStreamer,
    directory_stream: DiskStreamer,
}

impl Fat16 {
    pub fn new(disk_id: usize) -> Self {
        // NOTE: Hardcoding the disk here.
        Self {
            header: FatH::default(),
            root_dir: mem::MaybeUninit::uninit(),
            cluster_stream: DiskStreamer::new(disk_id),
            fat_stream: DiskStreamer::new(disk_id),
            directory_stream: DiskStreamer::new(disk_id),
        }
    }

    pub fn resolve(disk: &Global<Disk>) -> Result<Dyn<dyn FileSystem>, IOError> {
        let id = { disk.lock().id };
        let mut fat16 = Dyn::new(Self::new(id));

        let header = FatH::new(id);

        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        fat16.header = header;
        let start = fat16.root_start();
        let size = fat16.header.primary_header.root_dir_entries as usize;
        fat16.root_dir =
            mem::MaybeUninit::new(FatDirectory::new(&mut fat16.directory_stream, start, size));

        Ok(fat16)
    }

    fn root_start(&self) -> usize {
        let primary_header = self.header.primary_header;

        (primary_header.fat_copies as usize * primary_header.sectors_per_fat as usize
            + primary_header.reserved_sectors as usize)
            * SECTOR_SIZE
    }
}

static mut FAT16: Global<Fat16> = Global::new(|| Fat16::new(0), "FAT16");

impl FileSystem for Fat16 {
    fn open(&self, _path: Path, _mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
        // TODO: Handle different modes
        todo!()
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
