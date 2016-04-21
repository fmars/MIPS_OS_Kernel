Binary file ./.init.c.swp matches
Binary file ./emul/bin.elf matches
./emul/bin.elf.txt:bfc01220 <read_sr>:
./emul/bin.elf.txt:read_sr():
./emul/bin.elf.txt:unsigned int read_sr(void)
./emul/bin.elf.txt:  sr=read_sr();
./emul/bin.elf.txt:bfc012a8:	0ff00488 	jal	bfc01220 <read_sr>
./emul/bin.elf.txt:  sr=read_sr();
./emul/bin.elf.txt:bfc01378:	0ff00488 	jal	bfc01220 <read_sr>
./emul/bin.elf.txt:  sr=read_sr();
./emul/bin.elf.txt:bfc01448:	0ff00488 	jal	bfc01220 <read_sr>
./emul/bin.elf.txt:	sr=read_sr();
./emul/bin.elf.txt:bfc01b2c:	0ff00488 	jal	bfc01220 <read_sr>
./env.c:unsigned int read_sr(void)
./env.c:  sr=read_sr();
./env.c:  sr=read_sr();
./env.c:  sr=read_sr();
Binary file ./env.o matches
./env.o.gkd:;; Function (read_sr) 
./env.o.gkd:                (call (mem:SI (symbol_ref:SI ("read_sr") [flags 0x3] <function_decl # read_sr>) [ S4 A32])
./env.o.gkd:                (call (mem:SI (symbol_ref:SI ("read_sr") [flags 0x3] <function_decl # read_sr>) [ S4 A32])
./env.o.gkd:                (call (mem:SI (symbol_ref:SI ("read_sr") [flags 0x3] <function_decl # read_sr>) [ S4 A32])
./init.c:extern unsigned int read_sr(void);
./init.c:	sr=read_sr();
Binary file ./init.o matches
./init.o.gkd:                (call (mem:SI (symbol_ref:SI ("read_sr") [flags 0x41] <function_decl # read_sr>) [ S4 A32])
