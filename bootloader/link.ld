OUTPUT_FORMAT("binary");

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
