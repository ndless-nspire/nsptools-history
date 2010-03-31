GCC = arm-elf-gcc -nostdlib
GCCFLAGS = -mcpu=arm7tdmi -DUSE_BASIC_SYSCALLS_TABLE
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

all: loader

loader: loader.s
	@rm -rf loader.bin
	$(GCC) $(GCCFLAGS) -o loader.o loader.S ../../components/interrupts.S ../../components/utils.S
	$(LD) $(LDFLAGS) -Ttext 0xA4000000 -o loader.elf loader.o
	$(OBJCOPY) -O binary loader.elf loader.bin
	rm -f loader.o loader.elf
	cd MakeLoader; make package; mv loader.tns ../../../res/loader.tns
	
.PHONY: cleanall

cleanall: clean

clean:
	@rm -rf loader.bin
