
ENTRY(_start_)
SECTIONS
{

  . = 0xBFC00000;
  . = ALIGN(4);
  .text : {
	. = 0;
	*(.text.entry)
	. = 0x380;
	*(.text.exc_entry)
	. = 0x400;
	*(.text.int_entry)
	. = 0x800;
    *(.text)

    }

  . = ALIGN(4);
  .rodata : { *(.rodata)}
  . = ALIGN(4);
  .data : { *(.data)}

  . = 0x90000000;
  . = ALIGN(4);
  __bss_start = .;
  .bss (NOLOAD) : { *(.bss) }
  __bss_end = .;
  _end = .;
		
}
