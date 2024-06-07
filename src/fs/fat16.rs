use core::{lazy::Lazy, mem::MaybeUninit};
use crate::disk::get_disk;
use crate::disk::Disk;

use crate::disk::DiskStreamer;
use super::IOError;
use super::{fat_private::{FatDirectoryItem, FatH}, FileSystem};

const FAT16_SIGNATURE: u8 = 0x29;
const FAT16_ENTRY_SIZE: u16 = 0x02;
const FAT16_BAD_SECTOR: u16 = 0xFF7;
const FAT16_UNUSED: u16 = 0x00;

const FAT_FILE_READ_ONLY    :u8 = 1 << 0;
const FAT_FILE_HIDDEN       :u8 = 1 << 1;
const FAT_FILE_SYSTEM       :u8 = 1 << 2;
const FAT_FILE_VOLUME_LABEL :u8 = 1 << 3;
const FAT_FILE_SUBDIRECTORY :u8 = 1 << 4;
const FAT_FILE_ARCHIVED     :u8 = 1 << 5;
const FAT_FILE_DEVICE       :u8 = 1 << 6;
const FAT_FILE_RESERVERED   :u8 = 1 << 7;

struct FatDirectory {
    item: FatDirectoryItem,
    total: u32,
    sector_pos: u32,
    ending_sector_pos: u32,
}

enum FatItem {
    Directory(FatDirectory),
    File(FatDirectoryItem),
}

struct FileDescriptor {
    item: FatItem,
    pos: u32,
}

struct FatPrivate {
    header: FatH,
    root_dir: MaybeUninit<FatDirectory>,
    cluster_stream: DiskStreamer<'static>,
    fat_stream: DiskStreamer<'static>,
    directory_stream: DiskStreamer<'static>,
}

impl FatPrivate {
    pub fn new() -> Self {
        Self {
            header: FatH::default(),
            root_dir: MaybeUninit::uninit(),
            cluster_stream: DiskStreamer::new(0),
            fat_stream: DiskStreamer::new(0),
            directory_stream: DiskStreamer::new(0)

        }
    }
}

struct Fat16(FatPrivate);
static mut FAT16: Lazy<Fat16> = Lazy::new(||
    Fat16(FatPrivate::new())
);

impl FileSystem for Fat16 {
    fn resolve(&self, disk: &Disk) -> Result<(), IOError> {
        let mut buf: [u8; 36 + 26] = [0; 36 + 26];
        let mut streamer = DiskStreamer::new(disk.id);
        streamer.read( &mut buf, 36 + 26);
        let header = FatH::from(&buf);
        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        Ok(())
    }
    fn open(&self) {
        todo!()
    }
    fn read(&self) {
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

pub fn resolve() {
    unsafe { FAT16.resolve(get_disk(0)).unwrap() };
}
