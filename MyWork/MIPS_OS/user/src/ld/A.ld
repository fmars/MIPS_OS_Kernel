ENTRY(_start)
SECTIONS
{
  . = 0x80100000;

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
