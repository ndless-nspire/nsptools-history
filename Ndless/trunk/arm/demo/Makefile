include ../../Makefile.config

NSPIRE_HARDWARE ?= NON_CAS

AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
GCC = arm-elf-gcc
GCCFLAGS = -mcpu=arm7tdmi -Os -Wall -W -fpic -fno-merge-constants -lm
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

all: demo

demo: main.c syscalls.c utils.c vector.c particle.c particle_system.c gravity_particles.c	
	@rm -rf loader.bin
	$(GCC) $(GCCFLAGS) -D $(NSPIRE_HARDWARE) main.c syscalls.c utils.c vector.c particle.c particle_system.c gravity_particles.c -o demo.elf
	$(OBJCOPY) -O binary demo.elf demo.bin
	../../tools/MakeTNS/MakeTNS demo.bin demo.tns
	rm -f demo.bin demo.elf
	mkdir -p ../res/$(NSPIRE_HARDWARE)
	mv -f demo.tns ../res/$(NSPIRE_HARDWARE)/demo.tns
	
.PHONY: clean

clean: cleanbin

cleanbin:
	@rm -rf demo.bin
