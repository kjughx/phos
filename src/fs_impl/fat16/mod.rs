mod fat_private;

use crate::{
    disk::{Disk, DiskStreamer, SECTOR_SIZE},
    fs::{FileDescriptor, FileMode, FileSystem, IOError},
    memory::DynArray,
    types::Global,
    Box,
};
use core::mem;

use fat_private::{FatDirectoryItem, FatH, FAT_DIRECTORY_ITEM_SIZE, FAT_HEADER_SIZE};

const FAT16_SIGNATURE: u8 = 0x29;
const FAT16_ENTRY_SIZE: u16 = 0x02;
const FAT16_BAD_SECTOR: u16 = 0xFF7;
const FAT16_UNUSED: u8 = 0xE0;

const FAT_FILE_READ_ONLY: u8 = 1 << 0;
const FAT_FILE_HIDDEN: u8 = 1 << 1;
const FAT_FILE_SYSTEM: u8 = 1 << 2;
const FAT_FILE_VOLUME_LABEL: u8 = 1 << 3;
const FAT_FILE_SUBDIRECTORY: u8 = 1 << 4;
const FAT_FILE_ARCHIVED: u8 = 1 << 5;
const FAT_FILE_DEVICE: u8 = 1 << 6;
const FAT_FILE_RESERVERED: u8 = 1 << 7;

#[repr(C, packed)]
struct FatDirectory {
    items: DynArray<FatDirectoryItem>,
    total: u32,
    start: usize,
    end: usize,
}

impl FatDirectory {
    pub fn new(streamer: &mut DiskStreamer, start: usize, count: usize) -> Self {
        let mut items = DynArray::new(count);
        for i in 0..count as isize {
            items[i] = FatDirectoryItem::new(streamer, start);
        }

        streamer.seek(start);
        let total = Self::get_total_items(streamer);

        Self {
            items,
            total,
            start,
            end: start + count * FAT_DIRECTORY_ITEM_SIZE,
        }
    }

    fn get_total_items(streamer: &mut DiskStreamer) -> u32 {
        let pos = streamer.pos(); // We have to rewind when done

        const SIZE: usize = mem::size_of::<FatDirectoryItem>();
        let mut buf: [u8; SIZE] = [0; SIZE];
        let mut count = 0;
        loop {
            streamer.read(&mut buf, SIZE);
            match buf[0] {
                0 => break,
                0xE5 => continue,
                _ => count += 1,
            }
        }

        streamer.seek(pos);
        count
    }
}

enum FatItem {
    Directory(FatDirectory),
    File(FatDirectoryItem),
}

impl FatItem {}

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

    fn validate_signature() -> bool {
        let mut buf: [u8; FAT_HEADER_SIZE] = [0; FAT_HEADER_SIZE];
        let mut streamer = DiskStreamer::new(0);
        streamer.read(&mut buf, FAT_HEADER_SIZE);
        let header = FatH::from(&buf);

        header.extended_header.signature == FAT16_SIGNATURE
    }

    fn root_start(&self) -> usize {
        let primary_header = self.header.primary_header;

        (primary_header.fat_copies as usize * primary_header.sectors_per_fat as usize
            + primary_header.reserved_sectors as usize)
            * SECTOR_SIZE
    }
}

static mut FAT16: Global<Fat16> = Global::new(|| Fat16::new(0), "FAT16");

impl FileSystem for Global<Fat16> {
    fn resolve(&mut self, disk: &Disk) -> Result<&dyn FileSystem, IOError> {
        let mut fat16 = self.lock();

        let id = { disk.lock().id };
        let header = FatH::new(id);

        if header.extended_header.signature != FAT16_SIGNATURE {
            return Err(IOError::NotOurFS);
        }

        fat16.header = header;
        let start = fat16.root_start();
        let size = fat16.header.primary_header.root_dir_entries as usize;
        fat16.root_dir =
            mem::MaybeUninit::new(FatDirectory::new(&mut fat16.directory_stream, start, size));

        #[allow(static_mut_refs)]
        unsafe {
            Ok(&FAT16)
        }
    }

    fn open(&self, _mode: FileMode) -> Result<Box<dyn FileDescriptor>, IOError> {
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

pub fn resolve(disk: &Disk) -> Result<&'static dyn FileSystem, IOError> {
    unsafe { FAT16.resolve(disk) }
}
