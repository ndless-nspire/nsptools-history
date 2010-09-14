# Must be built on host. Must not be built for release.

AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os

all: crt0.o crt0sym.o osstub.o

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

clean:
	rm -rf *.o
