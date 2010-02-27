NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants
LDFLAGS = 
OBJCOPY = arm-elf-objcopy
OBJ = main.o syscalls.o utils.o vector.o particle.o particle_system.o gravity_particles.o

all: demo

.c.o:
	$(GCC) $(GCCFLAGS) -c -D $(NSPIRE_HARDWARE) $<

demo: $(OBJ)
	@rm -rf $@.bin
	$(GCC) $(GCCFLAGS) $(LDFLAGS) $^ -o $@.elf
	$(OBJCOPY) -O binary $@.elf $@.bin
	mkdir -p ../../res/$(NSPIRE_HARDWARE)
	MakeTNS $@.bin ../../res/$(NSPIRE_HARDWARE)/$@.tns
	rm -f $@.bin $@.elf

.PHONY: clean

clean:
	@rm -rf *.bin *.elf *.o
