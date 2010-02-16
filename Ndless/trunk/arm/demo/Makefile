NSPIRE_HARDWARE ?= NON_CAS

AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
GCC = arm-elf-gcc
GCCFLAGS = -mcpu=arm7tdmi -Os -Wall -W -fpic -fno-merge-constants
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy
HEADERSDIR=../headers
SYSTEMDIR=../system
TOOLSDIR=../../tools
OBJ = main.o syscalls.o utils.o vector.o particle.o particle_system.o gravity_particles.o

all: demo

.c.o:
	$(GCC) $(GCCFLAGS) -c -I$(HEADERSDIR) -D $(NSPIRE_HARDWARE) $<

demo: $(OBJ)
	@rm -rf $@.bin
	$(GCC) $(GCCFLAGS) $(LDFLAGS) -T $(SYSTEMDIR)/ldscript $(SYSTEMDIR)/crt0.s $^ -o $@.elf
	$(OBJCOPY) -O binary $@.elf $@.bin
	mkdir -p ../../res/$(NSPIRE_HARDWARE)
	$(TOOLSDIR)/MakeTNS/MakeTNS $@.bin ../../res/$(NSPIRE_HARDWARE)/$@.tns
	rm -f $@.bin $@.elf

.PHONY: clean

clean:
	@rm -rf *.bin *.elf *.o
