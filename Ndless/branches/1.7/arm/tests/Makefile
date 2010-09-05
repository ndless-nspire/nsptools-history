GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = tests.o
DISTDIR = ../../calcbin
vpath %.tns $(DISTDIR)

all: ndless_tests.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

ndless_tests.tns: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/ndless_tests.tns
