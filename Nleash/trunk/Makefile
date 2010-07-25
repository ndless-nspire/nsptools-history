NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
AS = nspire-as
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
OBJCOPY = arm-elf-objcopy
vpath %.tns  bin/$(NSPIRE_HARDWARE)

all: nleash.tns

dist:
	make NSPIRE_HARDWARE=NON_CAS
	make clean
	make NSPIRE_HARDWARE=CAS
	make clean

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

nleash.tns: nleash.o
	$(LD) -nostdlib $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	mkdir -p bin/$(NSPIRE_HARDWARE)
	cd MakeLoader && make loader && mv nleash.tns ../bin/$(NSPIRE_HARDWARE)

.PHONY: clean

clean:
	rm -rf bin *.bin *.elf *.o
