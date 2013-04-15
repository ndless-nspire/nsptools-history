GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS =
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = usb.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: ukbd.tns ukbd-azerty.tns ums.tns

ukbd.tns: ukbd.o
ums.tns: ums.o
ukbd-azerty.tns: ukbd.tns
	cp $< $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.tns: %.o
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f "$(DISTDIR)/*.tns"
