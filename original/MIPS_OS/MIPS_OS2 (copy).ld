
ENTRY(setsp)
SECTIONS
{

  . = 0xBFB00000;
  .int : {*(.text.int)
        }

  . = 0xBFC00000;
  .text : {
        . = 0;
        */start.o(.text)
        . = 0x380;
        */int_entry.o(.text)
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
