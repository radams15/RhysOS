OUTPUT_FORMAT("elf32-i386");

ENTRY(_main);

SECTIONS
{
    OVERLAY (0x1000) : AT (0x1000) {
        .text
        {
            _text_begin_addr = .;
            *(.text);
            _text_end_addr = .;
        }

        .data
        {
            _data_begin_addr = .;
            *(.data);
            *(.rodata*);
            *(.bss);
            _data_end_addr = .;
            _heap_begin_addr = . + 0x100;
            _heap_end_addr = 0x8000;
        }
    }
}
