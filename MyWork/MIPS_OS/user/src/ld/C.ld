ENTRY(_start)
SECTIONS
{
  . = 0x80300000;
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
