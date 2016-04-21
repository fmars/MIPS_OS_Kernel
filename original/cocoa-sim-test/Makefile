.PHONY : all
CROSS = mipsel-gnu-linux-
CC = $(CROSS)gcc
LD = $(CROSS)ld
AS = $(CROSS)as
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
CFLAGS = -Wall -g -fno-builtin -fno-pic -mno-abicalls
ASFLAGS = -gstabs -fno-pic -mno-abicalls -D__ASSEMBLY__
LDFLAGS = -T mips.lds
COPYFLAGS = --gap-fill=0xff -O binary
BINELF = bin.elf
BINBIN = bin.bin
SOURCES = head.S test.c

# .c -> .o, .s -> .o
OBJECTS = $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS)

all : $(BINBIN)
$(BINBIN) : $(BINELF)
	$(OBJCOPY) $(COPYFLAGS) $^ $@

$(BINELF) : $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^
	$(OBJDUMP) -lDS $@ > $@.txt
	
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o : %.S
	$(CC) $(ASFLAGS) -c $< -o $@

ALLDEP : $(SOURCES)
	$(CC) $(CFLAGS) -MM $^ > Makefile.dep
sinclude Makefile.dep

clean:
	rm -vf $(OBJECTS) $(BIN)
