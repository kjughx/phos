use core::ptr::slice_from_raw_parts;

macro_rules! u16 {
    ($n:ident, $o:literal) => {
        ($n[$o] as u16) << 8 | $n[$o + 1] as u16;
    };
}

macro_rules! u32 {
    ($n:ident, $o:literal) => {
        ($n[$o] as u32) << 24 | ($n[$o + 1] as u32) << 16 | ($n[$o + 2] as u32) << 8 | $n[$o + 3] as u32;
    };
}

#[repr(C, packed)]
#[derive(Default)]
pub struct FatHeaderExt {
    pub drive_no: u8,
    pub win_nt_bit: u8,
    pub signature: u8,
    pub volume_id: u32,
    pub volume_id_string: [u8; 11],
    pub system_id_string: [u8; 8],
}

impl From<&[u8]> for FatHeaderExt {
    fn from(bytes: &[u8]) -> Self {
        Self {
            drive_no: bytes[0],
            win_nt_bit: bytes[1],
            signature: bytes[2],
            volume_id: u32!(bytes, 3),
            volume_id_string: [
                bytes[7],
                bytes[8],
                bytes[9],
                bytes[10],
                bytes[11],
                bytes[12],
                bytes[13],
                bytes[14],
                bytes[15],
                bytes[16],
                bytes[17],
            ],
            system_id_string: [
                bytes[18],
                bytes[19],
                bytes[20],
                bytes[21],
                bytes[22],
                bytes[23],
                bytes[24],
                bytes[25],
            ]
        }
    }
}

#[repr(C, packed)]
#[derive(Default)]
pub struct FatHeader {
    short_jmp_ins: [u8; 3],
    oem_identifier: [u8; 8],
    bytes_per_sector: u16,
    sectors_per_cluster: u8,
    reseverd_sectors: u16,
    fat_copes:u8,
    root_dir_entries: u16,
    number_of_sectors: u16,
    media_type: u8,
    sectors_per_fat: u16,
    sectors_per_track: u16,
    number_of_heads: u16,
    hidden_sectors: u32,
    sectors_big: u32,
}

impl From<&[u8]> for FatHeader {
    fn from(bytes: &[u8]) -> Self {
        let short_jmp_ins: [u8; 3] = [bytes[0], bytes[1], bytes[2]];
        let oem_identifier: [u8; 8] = [
                bytes[3],
                bytes[4],
                bytes[5],
                bytes[6],
                bytes[7],
                bytes[8],
                bytes[9],
                bytes[10],
            ];
        Self {
            short_jmp_ins,
            oem_identifier,
            bytes_per_sector:  u16!(bytes, 11),
            sectors_per_cluster:  bytes[13],
            reseverd_sectors:  u16!(bytes, 14),
            fat_copes:  bytes[16],
            root_dir_entries:  u16!(bytes, 17),
            number_of_sectors:  u16!(bytes, 19),
            media_type:  bytes[21],
            sectors_per_fat:  u16!(bytes, 22),
            sectors_per_track:  u16!(bytes, 24),
            number_of_heads:  u16!(bytes, 26),
            hidden_sectors:  u32!(bytes, 28),
            sectors_big:  u32!(bytes, 32),
        }
    }
}

#[derive(Default)]
pub struct FatH {
    pub primary_header: FatHeader,
    pub extended_header: FatHeaderExt,
}

impl From<&[u8; 62]> for FatH {
    fn from(bytes: &[u8; 62]) -> Self {
        let (primary, extended) = unsafe {
            (
                core::mem::transmute_copy(&bytes),
                core::mem::transmute_copy(&&bytes[36..62]),
            )
        };
        Self {
            primary_header: primary,
            extended_header: extended,
        }
    }
}

#[repr(C, packed)]
pub struct FatDirectoryItem {
    filename: [u8; 8],
    extension: [u8; 3],
    attributes: u8,
    reserved: u8,
    creation_time_ds: u8,
    creation_time: u16,
    creation_dat: u16,
    last_access: u16,
    high_16_bits_first_cluster: u16,
    last_mod_time: u16,
    last_mod_data: u16,
    low_16_bits_first_cluster: u16,
    filesize: u32,
}
