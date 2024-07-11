const PROGRAM_VIRTUAL_ADDRESS: usize = 0x400000;
const USER_DATA_SEGMENT: usize = 0x23;
const USER_CODE_SEGMENT: usize = 0x1B;
const PROGRAM_VIRTUAL_STACK_START: usize = 0x3FF000;

pub struct Registers {
    edi: usize,
    esi: usize,
    ebp: usize,
    ebx: usize,
    edx: usize,
    ecx: usize,
    eax: usize,

    ip: usize,
    cs: usize,
    flags: usize,
    esp: usize,
    ss: usize,
}

impl Default for Registers {
    fn default() -> Self {
        Self {
            edi: 0,
            esi: 0,
            ebp: 0,
            ebx: 0,
            edx: 0,
            ecx: 0,
            eax: 0,

            ip: PROGRAM_VIRTUAL_ADDRESS,
            cs: USER_CODE_SEGMENT,
            flags: 0,
            esp: PROGRAM_VIRTUAL_STACK_START,
            ss: USER_DATA_SEGMENT,
        }
    }
}
