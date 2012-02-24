GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS = 
OBJCOPY := "$(shell (which arm-elf-objcopy arm-none-eabi-objcopy arm-linux-gnueabi-objcopy | head -1) 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = ndless_tests.test.tns ndless_keys.tns ndless_tpad.tns
DISTDIR = ../../calcbin/tests
vpath %.tns $(DISTDIR)

all: distdir $(EXE) static

distdir:
	@mkdir -p $(DISTDIR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

ndless_tests.test.tns: ndless_tests.o assert.o ../config.o
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@
	
%.tns: %.o
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

static:
	@cp ReadMe.txt $(DISTDIR)
	@cp ndless.cfg.tns $(DISTDIR)

clean:
	rm -f *.o *.elf
	-(cd $(DISTDIR) && rm -f $(EXE))
	rm -f $(DISTDIR)/ReadMe.txt $(DISTDIR)/ndless.cfg.tns
