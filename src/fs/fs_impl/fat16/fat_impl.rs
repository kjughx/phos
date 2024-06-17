use crate::{boxed::DynArray, disk::DiskStreamer};
use core::mem;

use super::fat_private::{FatDirectoryItem, FAT_DIRECTORY_ITEM_SIZE};

pub(super) const FAT16_SIGNATURE: u8 = 0x29;
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
pub(super) struct FatDirectory {
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
