OUTPUT_FORMAT("binary");

ENTRY(main);

SECTIONS
{
    . = ADDRESS;
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
