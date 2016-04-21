
OUTPUT_ARCH(mips)
ENTRY(_start)
SECTIONS
{
  . = 0xbfc00000;
  .text : 
  {
    head.o(.text)
    *(.text) 
  }
  . = ALIGN(4);
  .rodata : { *(.rodata)}
  . = ALIGN(4);
  .data : { *(.data)}

  . = ALIGN(4);
  __bss_start = .;
  .sbss (NOLOAD) : { *(.sbss) }
  .bss (NOLOAD) : { *(.bss) }
  __bss_end = .;
  _end = .;
}

