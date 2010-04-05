GCC = arm-elf-gcc -nostdlib
GCCFLAGS = -mcpu=arm7tdmi
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

all: ndless

ndless: ndless.S
	@rm -rf loader.bin
	$(GCC) $(GCCFLAGS) -o ndless.o ndless.S ../../components/interrupts.S ../../components/utils.S ../../components/files.S
	$(LD) $(LDFLAGS) -Ttext 0x11E11000 -o ndless.elf ndless.o
	$(OBJCOPY) -O binary ndless.elf ndless.bin
	rm -f ndless.o ndless.elf
	mv ndless.bin ../../res/ndless.tns
	
.PHONY: cleanall

cleanall: clean

clean:
	@rm -rf ndless.bin
