# Must be built on host. Must not be built for release.

AS = nspire-as
GCC = nspire-gcc
GXX = nspire-g++
GCCFLAGS = -Os

all: crt0.o crt0light.o crt0lightnoheader.o crt0-bflt.o crt0-cpp.o crt0-bflt-cpp.o crtn.o crti.o crt0sym.o osstub.o cppstub.o

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $<
	
%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

crt0light.o: crt0.S
	$(AS) $(GCCFLAGS) -D _NDLS_LIGHT -c $< -o $@

crt0lightnoheader.o: crt0.S
	$(AS) $(GCCFLAGS) -D _NDLS_LIGHT -D NO_PRGM_HEADER -c $< -o $@

crt0-cpp.o: crt0.S
	$(AS) $(GCCFLAGS) -D CRT0_CPP -c $< -o $@

crt0-bflt-cpp.o: crt0-bflt.S
	$(AS) $(GCCFLAGS) -D CRT0_CPP -c $< -o $@

clean:
	rm -rf *.o
