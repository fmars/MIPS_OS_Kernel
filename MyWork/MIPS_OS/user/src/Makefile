
.PHONY : all clean  clear

Env = A

LIB_SOURCES = start.S  syscall.S lib.c
USER_SOURCES =  $(LIB_SOURCES) $(Env).c  

CROSS = /home/pds/Work/MIPS/cross/cross/bin/mipsel-gnu-linux-
CC = $(CROSS)gcc
LD = $(CROSS)ld
AS = $(CROSS)as
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
INCLUDE = -I /usr/include
XXX = -fdump-final-insns
CFLAGS = $(XXX) -O0 -Wall -g -fno-builtin -fno-pic -mno-abicalls $(INCLUDE)
ASFLAGS = -gstabs -fno-pic -mno-abicalls -D__ASSEMBLY__ $(INCLUDE)

LDFLAGS = -T ld/$(Env).ld

DUMPFLAGS = -lDS
COPYFLAGS = --gap-fill=0xff -O binary
BINELF = ./bin/$(Env).elf
BINELF2 = ../tools/$(Env).elf
BINBIN = ./bin/$(Env).bin

SOURCES = $(USER_SOURCES)  
ALLDEP = Makefile.dep


# .c -> .o, .s -> .o
OBJECTS = $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS)

#all : $(ALLDEP) $(BINBIN)

all : $(BINBIN)  $(BINELF2)
$(BINBIN) : $(BINELF)
	$(OBJCOPY) $(COPYFLAGS) $^ $@

$(BINELF) : $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^
	$(OBJDUMP) $(DUMPFLAGS) $@ > $@.txt

$(BINELF2) : $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o : %.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(ALLDEP) : $(SOURCES)
	$(CC) $(CFLAGS) -MM $^ > $@

sinclude $(ALLDEP)

clean:
	rm -vf $(OBJECTS)  $(ALLDEP) *.o.gkd

clear:
	rm -vf $(OBJECTS)  $(ALLDEP) *.o.gkd $(BINBIN) $(BINELF) $(BINELF2) 




