include ../../Makefile.config

NSPIRE_HARDWARE ?= NON_CAS

AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
GCC = arm-elf-gcc
GCCFLAGS = -mcpu=arm7tdmi -Os -Wall -W -fpic -fno-merge-constants -lm
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy
SRC = main.c syscalls.c utils.c vector.c particle.c particle_system.c gravity_particles.c

all: demo

demo: $(SRC)	
	@rm -rf $@.bin
	$(GCC) $(GCCFLAGS) -D $(NSPIRE_HARDWARE) $(SRC) -o $@.elf
	$(OBJCOPY) -O binary $@.elf $@.bin
	../../tools/MakeTNS/MakeTNS $@.bin ../res/$(NSPIRE_HARDWARE)/$@.tns
	rm -f $@.bin $@.elf

.PHONY: clean

clean: cleanbin

cleanbin:
	@rm -rf demo.bin
