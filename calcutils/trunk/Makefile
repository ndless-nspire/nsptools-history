NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
LDFLAGS = -nostdlib
OBJCOPY = arm-elf-objcopy
vpath %.tns  bin/$(NSPIRE_HARDWARE)

all: boot2nanddumper.tns osreboot.tns diagsdumper.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.s
	$(GCC) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@

%.tns: %.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	MakeTNS $(@:.tns=.bin)
	rm -f $(@:.tns=.bin)
	mkdir -p bin/$(NSPIRE_HARDWARE)
	mv $@ bin/$(NSPIRE_HARDWARE)

boot2nanddumper.tns: boot2nanddumper.o
osreboot.tns: osreboot.o
diagsdumper.tns: diagsdumper.o

clean:
	@rm -rf *.bin *.elf *.o *.tns bin/$(NSPIRE_HARDWARE)
