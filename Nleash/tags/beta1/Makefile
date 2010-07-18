NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
AS = nspire-as
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
OBJCOPY = arm-elf-objcopy

all: nleash.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

nleash.tns: main.o
	$(LD) -nostdlib $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	cd MakeLoader && make loader && mv nleash.tns ..

.PHONY: clean

clean:
	@rm -rf *.tns *.elf *.bin *.o
