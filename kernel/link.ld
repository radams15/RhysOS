OUTPUT_FORMAT("elf32-i386");

ENTRY(main);

SECTIONS
{
    . = 0;
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
}
