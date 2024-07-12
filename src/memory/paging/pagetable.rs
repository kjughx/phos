use crate::prelude::*;

use super::{Addr, Flags, Offset, Page, ENTRIES_PER_TABLE, PAGE_SIZE};
use crate::{alloc, free};

#[derive(Clone, Copy)]
pub struct PageTableEntry(usize);
pub struct PageTable(pub *mut PageTableEntry);

pub const ENTRY_SIZE: usize = core::mem::size_of::<PageTableEntry>();
pub const TABLE_SIZE: usize = ENTRIES_PER_TABLE * ENTRY_SIZE;

impl PageTableEntry {
    pub fn new(addr: Addr, flags: Flags) -> Self {
        Self(addr.raw() | flags as usize)
    }

    pub fn addr(&self) -> Addr {
        Addr(self.0 & 0xfffff000)
    }

    pub fn flags(&self) -> Flags {
        (self.0 & 0x00000fff) as Flags
    }
}

impl PageTable {
    pub fn new(offset: Page, flags: Flags) -> Self {
        let table: *mut PageTableEntry = alloc(ENTRIES_PER_TABLE * ENTRY_SIZE);
        trace!("table address: {:x}", table as usize);
        for entry in 0..ENTRIES_PER_TABLE {
            let addr = Addr(offset.0 + entry * PAGE_SIZE);
            unsafe {
                table.add(entry).write(PageTableEntry::new(addr, flags));
            }
        }

        // Cursed??
        Self((table as usize | flags as usize) as *mut PageTableEntry)
    }

    pub fn free(self) {
        free(self.0);
    }

    pub fn from_ptr(ptr: *mut PageTable) -> Self {
        Self(((ptr as usize) & 0xfffff000) as *mut PageTableEntry)
    }

    pub fn get(&self, offset: Offset) -> PageTableEntry {
        unsafe { *self.0.add(offset.0) }
    }

    pub fn set(&mut self, offset: Offset, entry: PageTableEntry) {
        unsafe { self.0.add(offset.0).write(entry) }
    }
}
