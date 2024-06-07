use core::lazy::Lazy;

use crate::memory::Dyn;
use crate::fs::FileSystem;
use crate::types::Mutex;
use crate::{spinuntil, spinwhile};

use crate::io::{outb, insb, insw};

const SECTOR_SIZE: usize = 512;

#[derive(Debug)]
pub enum IOError {
    Other,
}

pub enum DiskType {
    Real
}

pub struct _Disk {
    r#type: DiskType,
    sector_size: usize,
    pub id: usize,

    filesystem: Option<Dyn<dyn FileSystem>>
}

impl _Disk {
    pub fn new(r#type: DiskType, sector_size: usize, id: usize) -> Self {
        Self {
            r#type, sector_size, id, filesystem: None,
        }
    }
    pub fn read_block(&self, lba: u32, total: u8, buf: &mut [u8]) {
        outb(0x1F6, ((lba >> 24) | 0xE0) as u8);
        outb(0x1F2, total);
        outb(0x1F3, (lba & 0xff) as u8);
        outb(0x1F4, (lba >> 8) as u8);
        outb(0x1F5, (lba >> 16) as u8);
        outb(0x1F7, 0x20);

        for _ in 0..total {
            spinwhile!(insb(0x1F7) & 0x08 == 0);

            for i in 0..SECTOR_SIZE/2 - 1 {
                let val = insw(0x1F0);
                buf[2*i] = (val & 0x00ff) as u8;
                buf[2*i + 1] = (val >> 8) as u8;
            }
        }
    }
}

pub type Disk = Lazy<_Disk>;

static mut DISK0: Disk = Lazy::new(||
    _Disk::new(DiskType::Real, SECTOR_SIZE, 0)
);

pub struct DiskStreamer<'a> {
    pos: usize,
    disk: &'a mut Disk,
}

impl<'a> DiskStreamer<'a> {
    pub fn new(id: usize) -> Self {
        unsafe {Self {
            pos: 0,
            disk: &mut DISK0
        }}
    }

    pub fn seek(&mut self, pos: usize) {
        self.pos = pos
    }

    pub fn read(&mut self, buf: &mut [u8], total: usize) {
        let mut bytes_read = 0;

        while bytes_read != total {
            let sector = self.pos / SECTOR_SIZE;
            let offset = self.pos % SECTOR_SIZE;
            let mut bytes_to_read = total - bytes_read;
            let mut local: [u8; SECTOR_SIZE] = [0; SECTOR_SIZE];

            if offset + bytes_to_read >= SECTOR_SIZE {
                // Read up to next sector boundary
                bytes_to_read -= (offset + bytes_to_read) - SECTOR_SIZE;
            }

            self.disk.read_block(sector as u32, 1, &mut local);

            for i in 0..bytes_to_read {
                buf[bytes_read + i] = local[i];
            }

            self.pos += bytes_to_read;
            bytes_read += bytes_to_read;
        }
    }
}

pub fn get_disk(id: usize) -> &'static Disk {
    unsafe { &DISK0 }
}
