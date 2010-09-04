AS = nspire-as
LD = nspire-ld
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = hella.o
DISTDIR = ../../calcbin/samples
vpath %.tns $(DISTDIR)

all: hella.tns

%.o: %.S
	$(AS) -c $<

hella.tns: $(OBJS)
	$(LD) -nostdlib $^ -o $(@:.tns=.elf)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/hella.tns
