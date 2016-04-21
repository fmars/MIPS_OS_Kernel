OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{

/*To do:
  fill in the correct address of the key section
  such as text data bss ...
*/
	.text 0x83000000 :{*(.text)}

}
