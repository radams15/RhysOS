OUTPUT_FORMAT("elf32-i386");

ENTRY(_crt0);

SECTIONS
{
    . = 0x1000;
    .text :
    {
        *(.text);
    }
    . = 0x6000;
    .data :
    {
        *(.data);
        *(.rodata*);
        *(.bss);
    }
    .comment :
    {
        *(.comment);
    }

    _heap_begin = 0x7000;
    _heap_end = _heap_begin + 0x1000;
}
