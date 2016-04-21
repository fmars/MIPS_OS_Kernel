
.PHONY : all clean

OS_SOURCES = console.c env.c env_asm.S genex.S init.c   pmap.c print.c \
              printf.c sched.c syscall.S syscall_all.S traps.c interupt.S start.S
CROSS = ../../../cross/bin/mipsel-gnu-linux-
CC = $(CROSS)gcc
LD = $(CROSS)ld
AS = $(CROSS)as
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
INCLUDE = -I /usr/include
XXX = -fdump-final-insns
CFLAGS = $(XXX) -O0 -Wall -g -fno-builtin -fno-pic -mno-abicalls $(INCLUDE)
ASFLAGS = -gstabs -fno-pic -mno-abicalls -D__ASSEMBLY__ $(INCLUDE)

LDFLAGS = -T MIPS_OS2.ld

DUMPFLAGS = -lDS
COPYFLAGS = --gap-fill=0xff -O binary
BINELF = ./emul/bin.elf
BINBIN = ./emul/bin.bin

#GLOBALSRCS = $(addprefix $(SHAREDIR)/, $(GLOBALSRC0))
#GLOBALSRC0 = head.S libc.c libasm.S

SOURCES = $(OS_SOURCES)  # $(GLOBALSRCS) 
ALLDEP = Makefile.dep


# .c -> .o, .s -> .o
OBJECTS = $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS)

#all : $(ALLDEP) $(BINBIN)
all : $(BINBIN) 
$(BINBIN) : $(BINELF)
	$(OBJCOPY) $(COPYFLAGS) $^ $@

$(BINELF) : $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^
	$(OBJDUMP) $(DUMPFLAGS) $@ > $@.txt
	
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o : %.S
	$(CC) $(ASFLAGS) -c $< -o $@

$(ALLDEP) : $(SOURCES)
	$(CC) $(CFLAGS) -MM $^ > $@

sinclude $(ALLDEP)

clean:
	rm -vf $(OBJECTS)  $(ALLDEP) *.o.gkd

