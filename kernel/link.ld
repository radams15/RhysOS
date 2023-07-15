OUTPUT_FORMAT("elf32-i386");

ENTRY(main);

SECTIONS
{
    . = 0;
    .text :
    {
        *(.text);
    }

    . = 0x5000;
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
}
