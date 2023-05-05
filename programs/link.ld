OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS
  {
    . = ADDRESS;
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
