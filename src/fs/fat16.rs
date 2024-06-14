use core::primitive;
use core::{lazy::Lazy, mem::MaybeUninit};
use crate::disk::get_disk;
use crate::disk::Disk;

use crate::disk::DiskStreamer;
use super::fat_private::FAT_HEADER_SIZE;
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

impl<'a> FatDirectory {
    fn new(disk: &Disk, sector_pos: usize, streamer: &'a mut DiskStreamer<'a>) -> Self {
        streamer.seek(sector_pos);

        let mut count = 0;
        loop {
            let mut buf = [0; FAT_DIRECTORY_ITEM_SIZE];
            streamer.read(&mut buf, FAT_DIRECTORY_ITEM_SIZE);
            match buf[0] {
                0 => break,
                0xE5 => continue,
                _ => count += 1,
            }
        }

        todo!()
    }
}

enum FatItem {
    Directory(FatDirectory),
    File(FatDirectoryItem),
}

struct FileDescriptor {
    item: FatItem,
    pos: u32,
}

struct Fat16<'a> {
    header: FatH,
    root_dir: MaybeUninit<FatDirectory>,
    cluster_stream: DiskStreamer<'a>,
    fat_stream: DiskStreamer<'a>,
    directory_stream: DiskStreamer<'a>,
}

static mut FAT16: Lazy<Fat16> = Lazy::new(Fat16::new);

impl<'a> Fat16<'a> {
    fn new(/* disk: &Disk */) -> Self {
        // NOTE: Hardcoding the disk here.
        Self {
            header: FatH::default(),
            root_dir: MaybeUninit::uninit(),
            cluster_stream: DiskStreamer::new(0),
            fat_stream: DiskStreamer::new(0),
            directory_stream: DiskStreamer::new(0)
        }
    }

    fn validate_signature(disk: &Disk) -> bool {
        let mut buf: [u8; FAT_HEADER_SIZE] = [0; FAT_HEADER_SIZE];
        let mut streamer = DiskStreamer::new(disk.id);
        streamer.read(&mut buf, FAT_HEADER_SIZE);
        let header = FatH::from(&buf);

        header.extended_header.signature == FAT16_SIGNATURE
    }

    fn root_offset(&self) -> usize {
        let primary_header = self.header.primary_header;

        primary_header.fat_copies as usize *  primary_header.sectors_per_fat as usize + primary_header.reserved_sectors as usize
    }

    fn populate_root(&'a mut self, disk: &Disk) -> Result<(), IOError> {
        let root_directory = FatDirectory::new(disk, self.root_offset(), &mut self.directory_stream);

        Ok(())
    }
}


impl FileSystem for Fat16<'_> {
    fn resolve(&self, disk: &Disk) -> Result<(), IOError> {
        if !Self::validate_signature(disk) {
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
