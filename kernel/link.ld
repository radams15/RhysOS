OUTPUT_FORMAT("elf32-i386");

ENTRY(_main);

SECTIONS
{
    . = 0x1000;
    .text :
    {
        *(.text);
    }

    . = 0x8000;
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
