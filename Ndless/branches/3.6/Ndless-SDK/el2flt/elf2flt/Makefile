srcdir = .

prefix = C:/Users/Olivier/Documents/Developpement/TI/Nspire/nsptools/Ndless/trunk/Ndless-SDK/yagarto
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

CC = gcc
CPU = arm
TARGET = arm-none-eabi
SYMBOL_PREFIX = 
CFLAGS = -Wall -g -O2
INCLUDES = -I$(srcdir) -Ic:/Users/Olivier/Downloads/binutils-2.21.tar/binutils-2.21/bfd -Ic:/Users/Olivier/Downloads/binutils-2.21.tar/binutils-2.21/include 
CPPFLAGS =  $(DEFS) $(INCLUDES)
LDFLAGS = 
LDLIBS = -lz c:/Users/Olivier/Downloads/binutils-2.21.tar/binutils-2.21/bfd/libbfd.a c:/Users/Olivier/Downloads/binutils-2.21.tar/binutils-2.21/libiberty/libiberty.a -lws2_32 -lz
INSTALL = /bin/install -c
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -D__EXTENSIONS__=1 -D_ALL_SOURCE=1 -D_GNU_SOURCE=1 -D_POSIX_PTHREAD_SEMANTICS=1 -D_TANDEM_SOURCE=1 -DHAVE_LIBZ=1 -DSTDC_HEADERS=1 -DHAVE_FCNTL_H=1 -DHAVE_UNISTD_H=1 -DHAVE_BFD_H=1 -DHAVE_VPRINTF=1 -DHAVE_STRSIGNAL=1 \
	-DTARGET_$(CPU) \
	-DTARGET_CPU=\"$(CPU)\" \
	-DSYMBOL_PREFIX=\"$(SYMBOL_PREFIX)\" \
	-DBINUTILS_LDSCRIPTDIR=\"${TOOLDIR}/../arm-none-eabi/lib\" \
	-DTARGET_ALIAS=\"$(TARGET)\" \
	-DNO_GOT_CHECK=1 \
	-DUSE_EMIT_RELOCS=1 \
	-DEMIT_CTOR_DTOR=0 \
	-DALWAYS_RELOC_TEXT=1
EXEEXT = .exe
OBJEXT = o

HOST = 

ifneq (,$(findstring mingw32,$(HOST)))
   LDLIBS += -lws2_32
endif

# force link order under cygwin to avoid getopts / libiberty clash
ifneq ($(strip $(shell gcc -v 2>&1 | grep "cygwin")),)
   LDLIBS := -lcygwin $(LDLIBS)
endif

LDFILE= elf2flt.ld
ifeq ($(strip $(CPU)),e1)
SRC_LDFILE= $(srcdir)/$(CPU)-elf2flt.ld
else 
SRC_LDFILE= elf2flt.ld
endif

target_bindir = $(prefix)/$(TARGET)/bin
target_libdir = $(prefix)/$(TARGET)/lib


PROG_ELF2FLT       = elf2flt$(EXEEXT)
PROG_FLTHDR        = flthdr$(EXEEXT)
PROG_LD_ELF2FLT_C  = ld-elf2flt$(EXEEXT)
PROG_LD_ELF2FLT_SH = ld-elf2flt.sh

ifeq (yes,yes)
PROG_LD_ELF2FLT    = $(PROG_LD_ELF2FLT_C)
else
PROG_LD_ELF2FLT    = $(PROG_LD_ELF2FLT_SH)
endif

PROGS = $(PROG_ELF2FLT) $(PROG_FLTHDR) $(PROG_LD_ELF2FLT_C) $(PROG_LD_ELF2FLT_SH)

all: $(PROGS)

# We need to declare the link explicitly because make only provides
# implicit rules when EXEEXT is set to nothing
link = $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(PROG_ELF2FLT): elf2flt.o compress.o stubs.o
	$(link)

$(PROG_FLTHDR): flthdr.o compress.o stubs.o
	$(link)

ld-elf2flt$(EXEEXT): ld-elf2flt.o stubs.o
	$(link)

ld-elf2flt.sh: $(srcdir)/ld-elf2flt.in
	./config.status $@
	chmod 755 $@

Makefile: $(srcdir)/Makefile.in
	./config.status $@

clean:
	-rm -f $(PROGS) *.$(OBJEXT) .deps

distclean: clean
	-rm -f Makefile config.log config.status config.cache ld-elf2flt

install:
	$(INSTALL) -d $(bindir)
	$(INSTALL) -d $(target_bindir)
	$(INSTALL) -d $(target_libdir)
	$(INSTALL) -m 755 $(PROG_FLTHDR) $(bindir)/$(TARGET)-$(PROG_FLTHDR)
	$(INSTALL) -m 755 $(PROG_FLTHDR) $(target_bindir)/$(PROG_FLTHDR)
	$(INSTALL) -m 755 $(PROG_ELF2FLT) $(bindir)/$(TARGET)-$(PROG_ELF2FLT)
	$(INSTALL) -m 755 $(PROG_ELF2FLT) $(target_bindir)/$(PROG_ELF2FLT)
	[ -f $(bindir)/$(TARGET)-ld.real$(EXEEXT) ] || \
		mv $(bindir)/$(TARGET)-ld$(EXEEXT) $(bindir)/$(TARGET)-ld.real$(EXEEXT)
	[ -f $(target_bindir)/ld.real$(EXEEXT) ] || \
		mv $(target_bindir)/ld$(EXEEXT) $(target_bindir)/ld.real$(EXEEXT)
	$(INSTALL) -m 755 $(PROG_LD_ELF2FLT) $(bindir)/$(TARGET)-ld$(EXEEXT)
	$(INSTALL) -m 755 $(PROG_LD_ELF2FLT) $(target_bindir)/ld$(EXEEXT)
	$(INSTALL) -m 644 $(SRC_LDFILE) $(target_libdir)/$(LDFILE)

sinclude .deps
.deps:
	$(CC) -MM $(CPPFLAGS) $(srcdir)/*.c > .deps
