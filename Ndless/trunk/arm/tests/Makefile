GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS = 
OBJCOPY := "$(shell (which arm-elf-objcopy arm-none-eabi-objcopy arm-linux-gnueabi-objcopy | head -1) 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
DISTDIR = ../../calcbin/tests
vpath %.tns $(DISTDIR)

all: distdir ndless_tests.tns ndless_keys.tns doc

distdir:
	@mkdir -p $(DISTDIR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.tns:
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

ndless_tests.tns: tests.o
ndless_keys.tns: keys.o

doc:
	@cp ReadMe.txt $(DISTDIR)

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/ndless_tests.tns
	rm -f $(DISTDIR)/ndless_keys.tns
	rm -f $(DISTDIR)/ReadMe.txt
