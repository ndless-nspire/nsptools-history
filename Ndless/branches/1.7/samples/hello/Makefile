GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -fno-merge-constants
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = hello.o
vpath %.tns  ../../res

all: hello.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

hello.tns: $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(@:.tns=.bin)
	MakeTNS $(@:.tns=.bin)
	rm -f $(@:.tns=.bin)
	mkdir -p ../../res
	mv $@ ../../res

clean:
	rm -f *.o *.elf
	rm -f ../../res/hello.tns
