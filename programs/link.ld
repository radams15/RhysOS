OUTPUT_FORMAT("elf32-i386");

ENTRY(_crt0);

SECTIONS
{
    . = 0x3000;
    .text :
    {
        *(.text);
    }
    
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

    _heap_begin = . + 0x10;
    _heap_end = 0x8000;
}
