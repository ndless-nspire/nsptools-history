NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
AS = nspire-as
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
OBJCOPY = arm-elf-objcopy
OBJS = installos.o
vpath %.tns  ./

all: installos.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

installos.tns: $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	MakeTNS $(@:.tns=.bin)
	rm -f $(@:.tns=.bin) 

.PHONY: clean

clean:

