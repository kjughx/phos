use crate::{disk::DiskStreamer, trace, DynArray};
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

#[derive(Clone)]
pub(super) struct FatDirectory {
    items: DynArray<FatDirectoryItem>,
    total: u32,
    start: usize,
    end: usize,
}

impl FatDirectory {
    pub fn new(streamer: &mut DiskStreamer, start: usize, count: usize) -> Self {
        streamer.seek(start);
        let total = Self::get_total_items(streamer);

        let mut items = DynArray::new(count);
        for _ in 0..total as isize {
            items.push(FatDirectoryItem::new(streamer))
        }

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

    pub fn find(&self, streamer: &mut DiskStreamer, name: &str) -> Option<FatItem> {
        for item in self.items.into_iter() {
            if item.filename() == name {
                return Some(FatItem::new(streamer, item));
            }
        }

        None
    }
}

pub(super) enum FatItem {
    Directory(FatDirectory),
    File(FatDirectoryItem),
}

impl FatItem {
    pub fn new(streamer: &mut DiskStreamer, item: &FatDirectoryItem) -> Self {
        match item.attributes {
            FAT_FILE_SUBDIRECTORY => {
                let size = FatDirectoryItem::size(streamer);
                FatItem::Directory(FatDirectory::new(streamer, item.first_cluster(), size))
            }
            _ => FatItem::File(*item),
        }
    }
}
