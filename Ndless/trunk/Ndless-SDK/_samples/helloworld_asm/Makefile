AS = nspire-as
LD = nspire-ld
OBJCOPY := "$(shell (which arm-elf-objcopy arm-none-eabi-objcopy arm-linux-gnueabi-objcopy | head -1) 2>/dev/null)"
EXE = helloworld_asm.tns
OBJS = $(patsubst %.S,%.o,$(wildcard *.S))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.S
	$(AS) -c $<

$(EXE): $(OBJS)
	$(LD) -nostdlib $^ -o $(@:.tns=.elf)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
