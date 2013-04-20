GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS =
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: usbhidn.tns usbhidn-azerty.tns

usbhidn.tns: hidn.o ums.o ukbd.o
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

usbhidn-azerty.tns: usbhidn.tns
	cp $< $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

clean:
	rm -f *.o *.elf
	rm -f "$(DISTDIR)/*.tns"
