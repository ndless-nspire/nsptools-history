AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os

all: crt0.o syscalls.o stdlibsym.o

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

clean:
	rm -rf *.o
