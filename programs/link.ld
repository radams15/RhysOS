OUTPUT_FORMAT("elf32-i386");

ENTRY(_crt0);

SECTIONS
{
    . = 0x1000;
    .text :
    {
        *(.text);
    }
    . = 0x2000;
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
