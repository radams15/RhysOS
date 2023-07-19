OUTPUT_FORMAT("elf32-i386");

ENTRY(_start);

SECTIONS
{
    . = 0x1000;
    .text :
    {
        *(.text);
    }
    . = ADDRESS;
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
