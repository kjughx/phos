ENTRY(_start)
SECTIONS
{
    . = 1M;
    .start :{
        *(.start)
        . = 1M + 512;
    } =0

    .text : {
        *(.text)
        *(.text.*)
    }

    .asm :{
        *(.asm)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}
