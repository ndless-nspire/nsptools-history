GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = hello.o
DISTDIR = ../../calcbin/samples
vpath %.tns $(DISTDIR)

all: hello.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

hello.tns: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $(@:.tns=.elf)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/hello.tns
