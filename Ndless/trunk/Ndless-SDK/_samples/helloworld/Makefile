GCC = nspire-gcc
LD = nspire-ld-bflt
GCCFLAGS = -O0 -g -Wall -W -marm
LDFLAGS = 
EXE = helloworld.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $(LDFLAGS) $^ -o $(DISTDIR)/$@

clean:
	rm -f *.o *.elf *.gdb
	rm -f $(DISTDIR)/$(EXE)
