AS = nspire-as
AR := "$(shell which arm-elf-ar 2>/dev/null)"
ifeq (${AR},"")
	AR := arm-none-eabi-ar
endif
GCC = nspire-gcc
GCCFLAGS = -Os -nostdlib -std=c99
LD = nspire-ld
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
LIB = libndls.a
DISTDIR = ../lib
vpath %.a $(DISTDIR)
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all: static $(LIB)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@

static:
	@mkdir -p $(DISTDIR)
	
$(LIB): $(OBJS)
	$(AR) rcs $(DISTDIR)/$(LIB) $^

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

clean:
	rm -rf *.o *.elf *.a
	rm -f $(DISTDIR)/$(LIB)
