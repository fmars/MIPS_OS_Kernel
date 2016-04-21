.PNONY : all all32 all64 install32 install64


all : all32 all64
all32 : sim32 flash32
all64 : sim64 flash64

sim32 : debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o
	gcc -c -m32 *.c
	gcc -m32 -static -o ../sim32 debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o
sim64 : debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o
	gcc -c -m64 *.c
	gcc -m64 -static -o ../sim64 debug.o cpu.o cp0.o address.o flash.o timer.o icu.o memory.o disass.o ass.o string.o

flash32 : flasher.o disass.o
	gcc -c -m32 *.c
	gcc -m32 -static -o ../flash32 flasher.o disass.o
flash64 : flasher.o disass.o
	gcc -c -m64 *.c
	gcc -m64 -static -o ../flash64 flasher.o disass.o

.PHONY : clear  clean
clear :
	rm -f *.o ../sim32 ../sim64 ../flash32 ../flash64
clean :
	rm -f *.o
install32 :
	echo "use root to install!"
	cp -f ./sim32 /usr/local/bin/cocoa-sim
	echo "simulator command : cocoa-sim"
	cp -f flash32 /usr/local/bin/cocoa-flash
	echo "flash-editor command : cocoa-flash"
install64 :
	echo "use root to install!"
	cp -f sim64 /usr/local/bin/cocoa-sim
	echo "simulator command : cocoa-sim"
	cp -f flash64 /usr/local/bin/cocoa-flash
	echo "flash-editor command : cocoa-flash"
