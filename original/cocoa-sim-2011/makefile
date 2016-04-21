.PNONY : all install
CC = gcc
CFLAGS = -Wall

all : debug flasher

debug : debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o
	$(CC) $(CFLAGS) -o debug debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o
vm : cpu.o address.o memory.o flash.o vm.o cp0.o timer.o icu.o disass.o iolet.o
	$(CC) $(CFLAGS) -o vm cpu.o address.o memory.o flash.o vm.o cp0.o timer.o icu.o disass.o iolet.o -lpthread
assembler : string.o disass.o assembler.o
	$(CC) $(CFLAGS) -o assembler string.o disass.o assembler.o
flasher : flasher.o disass.o
	$(CC) $(CFLAGS) -o flasher flasher.o disass.o
address.o : address.c  address.h memory.h flash.h cp0.h cpu.h timer.h icu.h
	$(CC) $(CFLAGS) -c -o address.o address.c
ass.o : ass.c  ass.h string.h cpu.h
	$(CC) $(CFLAGS) -c -o ass.o ass.c
assembler.o : assembler.c  string.h disass.h
	$(CC) $(CFLAGS) -c -o assembler.o assembler.c
cp0.o : cp0.c  cp0.h cpu.h icu.h
	$(CC) $(CFLAGS) -c -o cp0.o cp0.c
cpu.o : cpu.c  cpu.h cp0.h address.h timer.h flash.h disass.h
	$(CC) $(CFLAGS) -c -o cpu.o cpu.c
debug.o : debug.c  debug.h cpu.h cp0.h address.h flash.h timer.h icu.h memory.h disass.h ass.h string.h
	$(CC) $(CFLAGS) -c -o debug.o debug.c
disass.o : disass.c  disass.h cpu.h
	$(CC) $(CFLAGS) -c -o disass.o disass.c
flash.o : flash.c  flash.h
	$(CC) $(CFLAGS) -c -o flash.o flash.c
flasher.o : flasher.c  flasher.h disass.h
	$(CC) $(CFLAGS) -c -o flasher.o flasher.c
icu.o : icu.c  icu.h cp0.h cpu.h
	$(CC) $(CFLAGS) -c -o icu.o icu.c
iolet.o : iolet.c  icu.h
	$(CC) $(CFLAGS) -c -o iolet.o iolet.c
memory.o : memory.c  memory.h
	$(CC) $(CFLAGS) -c -o memory.o memory.c
string.o : string.c  string.h
	$(CC) $(CFLAGS) -c -o string.o string.c
timer.o : timer.c  timer.h icu.h
	$(CC) $(CFLAGS) -c -o timer.o timer.c
uart.o : uart.c  uart.h
	$(CC) $(CFLAGS) -c -o uart.o uart.c
vm.o : vm.c  cpu.h address.h memory.h flash.h vm.h cp0.h timer.h icu.h
	$(CC) $(CFLAGS) -c -o vm.o vm.c
.PHONY : clean
clean :
	rm -f *.o debug vm assembler flasher
install :
	echo "use root to install!"
	cp -f debug /usr/local/bin/cocoa-sim-debug
	echo "simulator command : cocoa-sim-debug"
	cp -f flasher /usr/local/bin/cocoa-flash-ed
	echo "flash-editor command : cocoa-flash-ed"

