use crate::{disk::DiskStreamer, Addr};

#[repr(C, packed)]
#[derive(Default, Clone, Copy)]
pub struct FatHeaderExt {
    drive_no: u8,
    win_nt_bit: u8,
    pub signature: u8,
    pub volume_id: u32,
    pub volume_id_string: [u8; 11],
    pub system_id_string: [u8; 8],
}

#[repr(C, packed)]
#[derive(Default, Clone, Copy)]
pub struct FatHeader {
    pub short_jmp_ins: [u8; 3],
    pub oem_identifier: [u8; 8],
    pub bytes_per_sector: u16,
    pub sectors_per_cluster: u8,
    pub reserved_sectors: u16,
    pub fat_copies: u8,
    pub root_dir_entries: u16,
    pub number_of_sectors: u16,
    pub media_type: u8,
    pub sectors_per_fat: u16,
    pub sectors_per_track: u16,
    pub number_of_heads: u16,
    pub hidden_sectors: u32,
    pub sectors_big: u32,
}

#[derive(Default, Clone, Copy)]
pub struct FatH {
    pub primary_header: FatHeader,
    pub extended_header: FatHeaderExt,
}

impl FatH {
    pub fn new(disk_id: usize) -> Self {
        let mut streamer = DiskStreamer::new(disk_id);
        const HEADER_SIZE: usize = core::mem::size_of::<FatH>();
        let mut buf: [u8; HEADER_SIZE] = [0; HEADER_SIZE];
        streamer.read(&mut buf, HEADER_SIZE);

        FatH::from(&buf)
    }
}

pub const FAT_HEADER_SIZE: usize = core::mem::size_of::<FatH>();

impl From<&[u8; FAT_HEADER_SIZE]> for FatH {
    fn from(bytes: &[u8; FAT_HEADER_SIZE]) -> Self {
        unsafe { *(bytes.as_ptr() as *const FatH) }
    }
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct FatDirectoryItem {
    pub filename: [u8; 8],
    pub extension: [u8; 3],
    pub attributes: u8,
    pub reserved: u8,
    pub creation_time_ds: u8,
    pub creation_time: u16,
    pub creation_dat: u16,
    pub last_access: u16,
    pub high_16_bits_first_cluster: u16,
    pub last_mod_time: u16,
    pub last_mod_data: u16,
    pub low_16_bits_first_cluster: u16,
    pub filesize: u32,
}

impl FatDirectoryItem {
    pub fn new(streamer: &mut DiskStreamer, start: usize) -> Self {
        let mut buf = [0; FAT_DIRECTORY_ITEM_SIZE];
        streamer.seek(start);
        streamer.read(&mut buf, FAT_DIRECTORY_ITEM_SIZE);
        FatDirectoryItem::from(&buf)
    }
    pub fn first_cluster(&self) -> Addr {
        Addr(self.high_16_bits_first_cluster as u32 | self.low_16_bits_first_cluster as u32)
    }
}

pub const FAT_DIRECTORY_ITEM_SIZE: usize = core::mem::size_of::<FatDirectoryItem>();
impl From<&[u8; FAT_DIRECTORY_ITEM_SIZE]> for FatDirectoryItem {
    fn from(bytes: &[u8; FAT_DIRECTORY_ITEM_SIZE]) -> Self {
        unsafe { *(bytes.as_ptr() as *const FatDirectoryItem) }
    }
}
