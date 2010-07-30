NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
OBJCOPY = arm-elf-objcopy
OBJS = main.o utils.o vector.o particle.o particle_system.o gravity_particles.o
vpath %.tns  ../../res/$(NSPIRE_HARDWARE)

all: demo.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

demo.tns: $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	MakeTNS $(@:.tns=.bin)
	rm -f $(@:.tns=.bin)
	mkdir -p ../../res/$(NSPIRE_HARDWARE)
	mv $@ ../../res/$(NSPIRE_HARDWARE) 

.PHONY: clean

clean:
	rm -f *.o *.elf
