use crate::prelude::*;

use crate::{lock, Error, Global};

use super::{
    pagetable::{PageTable, PageTableEntry, TABLE_SIZE},
    r#extern::load_directory,
    Addr, Flags, Page, ENTRIES_PER_TABLE, PAGE_SIZE,
};

#[derive(Clone, Copy)]
pub struct PageDirectory(*mut PageTable);

static mut CURRENT_DIRECTORY: Global<PageDirectory> =
    Global::new(|| PageDirectory::new(0), "CURRENT PAGE DIRECTORY");

impl PageDirectory {
    pub fn new(flags: Flags) -> Self {
        let tables: *mut PageTable = alloc(ENTRIES_PER_TABLE * TABLE_SIZE);

        for entry in 0..ENTRIES_PER_TABLE {
            unsafe {
                tables.add(entry).write(PageTable::new(entry, flags));
            }
        }

        Self(tables)
    }

    fn free(self) {
        for i in 0..ENTRIES_PER_TABLE {
            let table = self.get_table(Page(i));
            table.free()
        }

        free(self.0)
    }

    pub fn load(&self) {
        let mut current_directory = unsafe { lock!(CURRENT_DIRECTORY) };
        load_directory(self.0 as *const usize);
        *current_directory = *self;
    }

    fn get_table(&self, page: Page) -> PageTable {
        PageTable::from_ptr(unsafe { self.0.add(page.0) })
    }

    fn set(&mut self, vaddr: Addr, entry: PageTableEntry) {
        assert!(vaddr.is_aligned());

        self.get_table(vaddr.as_page())
            .set(vaddr.as_offset(), entry);
    }

    pub fn map(&mut self, vaddr: Addr, paddr: Addr, flags: Flags) -> Result<(), Error> {
        if !vaddr.is_aligned() || !paddr.is_aligned() {
            return Err(Error::InvalidArgument);
        }

        self.set(vaddr, PageTableEntry::new(paddr, flags));

        Ok(())
    }

    pub fn map_range(
        &mut self,
        vstart: Addr,
        pstart: Addr,
        pend: Addr,
        flags: Flags,
    ) -> Result<(), Error> {
        if !vstart.is_aligned()
            || !pstart.is_aligned()
            || pend.is_aligned()
            || pend.raw() < pstart.raw()
        {
            return Err(Error::InvalidArgument);
        }

        let count = (pend.raw() - pstart.raw()) / PAGE_SIZE;
        for page in 0..count {
            self.map(
                vstart.offset(page * PAGE_SIZE),
                pstart.offset(page * PAGE_SIZE),
                flags,
            )?;
        }

        Ok(())
    }

    pub fn get_entry(&self, vaddr: Addr) -> Option<PageTableEntry> {
        if !vaddr.is_aligned() {
            return None;
        }

        Some(self.get_table(vaddr.as_page()).get(vaddr.as_offset()))
    }

    pub fn get_paddr(&self, vaddr: Addr) -> Addr {
        self.get_table(vaddr.align_lower().as_page())
            .get(vaddr.as_offset())
            .addr()
    }

    pub fn get_flags(&self, vaddr: Addr) -> Flags {
        self.get_table(vaddr.align_lower().as_page())
            .get(vaddr.as_offset())
            .flags()
    }
}
