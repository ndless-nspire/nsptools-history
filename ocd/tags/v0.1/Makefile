GCC = nspire-gcc
AR := "$(shell (which arm-elf-ar arm-none-eabi-ar arm-linux-gnueabi-ar | head -1) 2>/dev/null)"
LD = nspire-ld
GCCFLAGS = -Os -nostdlib -Wall -W -marm -I nspireio
LDFLAGS = -nostdlib -L nspireio -lnspireio2
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
LIB = libocd.a
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(LIB)

dist: src all
	cp $(DISTDIR)/$(LIB) ocd.h ReadMe.txt dist

src: clean
	rm -rf dist
	mkdir -p dist/src
	cp *.c *.h Makefile dist/src

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(LIB): $(OBJS)
	mkdir -p $(DISTDIR)
	$(AR) rcs $(DISTDIR)/$(LIB) $^

clean:
	rm -rf dist
	rm -f *.o *.elf *.a
	rm -f $(DISTDIR)/$(LIB)
