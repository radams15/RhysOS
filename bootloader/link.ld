OUTPUT_FORMAT("binary");

ENTRY(_main);

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
