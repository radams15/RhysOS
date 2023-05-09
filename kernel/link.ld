OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS
  {
    . = 0x1000;
    .text : {
		*(.text) *(.text.*)
	}
    .data : {
		*(.rodata) *(.rodata.*)
		*(.data) *(.data.*)
		*(.gcc_except_table)
	}
    .bss (NOLOAD) : {
                *(.bss) *(.bss.*)
                *(COMMON)
	}
    /DISCARD/ : { *(.*) }
  }
