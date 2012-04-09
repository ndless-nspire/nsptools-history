GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm -I nspireio -I "C:\Users\Olivier\Documents\Developpement\TI\Nspire\nsptools\ocd\trunk"
LDFLAGS = -L nspireio -lnspireio2 -L "C:\Users\Olivier\Documents\Developpement\TI\Nspire\nsptools\ocd\trunk" -locd
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = usb.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
