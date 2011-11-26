# Must be built on host. Must not be built for release.

AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os

all: crt0.o crt0light.o crt0lightnoheader.o crt0sym.o osstub.o

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

crt0light.o: crt0.S
	$(AS) $(GCCFLAGS) -D _NDLS_LIGHT -c $< -o $@

crt0lightnoheader.o: crt0.S
	$(AS) $(GCCFLAGS) -D _NDLS_LIGHT -D NO_PRGM_HEADER -c $< -o $@

clean:
	rm -rf *.o
