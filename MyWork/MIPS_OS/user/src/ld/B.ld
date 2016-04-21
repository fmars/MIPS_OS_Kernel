ENTRY(_start)
SECTIONS
{
  . = 0x80200000;
  .text : {
	. = 0;
	*(.text.entry)
        *(.text)
        }
  .date : {
	*(.data)
	}
  .bss  : {
	*(.dynbss)
	*(.bss)
  }
   .got : {
	*(.got)
  }
   end = . ;
}
