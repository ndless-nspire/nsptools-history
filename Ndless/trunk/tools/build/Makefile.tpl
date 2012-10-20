GCC = nspire-gcc
LD = nspire-ld-bflt
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS = 
EXE = @@EXENAME@@.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
