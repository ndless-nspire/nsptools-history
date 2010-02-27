NSPIRE_HARDWARE ?= NON_CAS

GCC = nspire-gcc
GCCFLAGS = -Os -Wall -W -fpic -fno-merge-constants -D $(NSPIRE_HARDWARE)
LDFLAGS = 
OBJCOPY = arm-elf-objcopy
OBJS = main.o syscalls.o utils.o vector.o particle.o particle_system.o gravity_particles.o
vpath %.tns  ../../res/$(NSPIRE_HARDWARE)

all: demo.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.s
	$(GCC) $(GCCFLAGS) -c $<

demo.tns: $(OBJS)
	$(GCC) $(GCCFLAGS) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	MakeTNS $(@:.tns=.bin)
	rm -f $(@:.tns=.bin)
	mkdir -p ../../res/$(NSPIRE_HARDWARE)
	mv $@ ../../res/$(NSPIRE_HARDWARE) 

.PHONY: clean

clean:
	@rm -rf *.bin *.elf *.o *.tns ../../res/$(NSPIRE_HARDWARE)/demo.tns
