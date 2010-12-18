GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -O0 -g -Wall -W
LDFLAGS = 
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = tests.o
DISTDIR = ../../calcbin/tests
vpath %.tns $(DISTDIR)

all: distdir ndless_tests.tns doc

distdir:
	@mkdir -p $(DISTDIR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

ndless_tests.tns: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

doc:
	@cp ReadMe.txt $(DISTDIR)

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/ndless_tests.tns
	rm -f $(DISTDIR)/ReadMe.txt
