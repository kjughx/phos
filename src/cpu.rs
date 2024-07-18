use core::fmt::Display;

use crate::packed::{packed, Packed};

const PROGRAM_VIRTUAL_ADDRESS: usize = 0x400000;
const USER_DATA_SEGMENT: usize = 0x23;
const USER_CODE_SEGMENT: usize = 0x1B;
const PROGRAM_VIRTUAL_STACK_START: usize = 0x3FF000;

#[allow(dead_code)]

pub type Registers = InterruptFrame;

impl Registers {
    pub fn user_default() -> Self {
        Self {
            edi: 0,
            esi: 0,
            ebp: 0,
            ebx: 0,
            edx: 0,
            ecx: 0,
            eax: 0,
            unused: 0,

            ip: PROGRAM_VIRTUAL_ADDRESS,
            cs: USER_CODE_SEGMENT,
            flags: 0,
            sp: PROGRAM_VIRTUAL_STACK_START,
            ss: USER_DATA_SEGMENT,
        }
    }

    pub fn save(&mut self, frame: InterruptFrame) {
        self.edi = frame.edi;
        self.esi = frame.esi;
        self.ebp = frame.ebp;
        self.ebx = frame.ebx;
        self.edx = frame.edx;
        self.ecx = frame.ecx;
        self.eax = frame.eax;
        self.ip = frame.ip;
        self.cs = frame.cs;
        self.flags = frame.flags;
        self.sp = frame.sp;
        self.ss = frame.ss;
    }
}

#[packed]
pub struct InterruptFrame {
    pub edi: usize,
    pub esi: usize,
    pub ebp: usize,
    pub unused: usize,
    pub ebx: usize,
    pub edx: usize,
    pub ecx: usize,
    pub eax: usize,
    pub ip: usize,
    pub cs: usize,
    pub flags: usize,
    pub sp: usize,
    pub ss: usize,
}

impl Display for InterruptFrame {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let edi = self.edi;
        let esi = self.esi;
        let ebp = self.ebp;
        let ebx = self.ebx;
        let edx = self.edx;
        let ecx = self.ecx;
        let eax = self.eax;
        let ip = self.ip;
        let cs = self.cs;
        let flags = self.flags;
        let sp = self.sp;
        let ss = self.ss;
        write!(
            f,
            r#"
            edi: 0x{:08x}    esi: 0x{:08x}    ebp: 0x{:08x}
            ebx: 0x{:08x}    edx: 0x{:08x}
            ecx: 0x{:08x}    eax: 0x{:08x}

            flags: 0b{:08b}
            ip: 0x{:08x}     cs: 0x{:08x}
            sp: 0x{:08x}
            ss: 0x{:08x}
        "#,
            edi, esi, ebp, ebx, edx, ecx, eax, flags, ip, cs, sp, ss
        )
    }
}
