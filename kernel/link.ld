OUTPUT_FORMAT("elf32-i386");

ENTRY(_main);

SECTIONS
{
    . = 0x1000;
    
    .text :
    {
        *(.text);
    }

    .data :
    {
        *(.data);
        *(.rodata*);
        *(.bss);
        
        _heap_begin_addr = . + 0x100;
        _heap_end_addr = 0x8000;
    }
}
