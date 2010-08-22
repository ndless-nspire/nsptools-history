NSPIRE_HARDWARE ?= NON_CAS

AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os -D $(NSPIRE_HARDWARE) -nostdlib -I "../include"
LD = nspire-ld
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
DISTDIR = ../res/$(NSPIRE_HARDWARE)
# OS-specific
SYSCALLS_LIGHT_OBJS=syscalls-light_ncas-1.7.o syscalls-light_cas-1.7.o
SYSCALLS_OBJS=syscalls_ncas-1.7.o syscalls_cas-1.7.o
vpath %.tns  $(DISTDIR)

all: static ndless_installer.tns ndless_resources.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@


static:
	@mkdir -p $(DISTDIR)

# Build with the light switch, because of the 1kb limit of ndless_installer.tns
ints_light.o: ints.c
	$(GCC) $(GCCFLAGS) -D _SYSCALLS_LIGHT -c $< -o $@
syscalls_light.o: syscalls.c
	$(GCC) $(GCCFLAGS) -D _SYSCALLS_LIGHT -c $< -o $@
utils_light.o: utils.c
	$(GCC) $(GCCFLAGS) -D _SYSCALLS_LIGHT -c $< -o $@

ndless_installer.elf: GCCFLAGS:=$(GCCFLAGS) -D _SYSCALLS_LIGHT
# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup 
ndless_installer.elf: bootstrapper.o loader.o ints_light.o syscalls_light.o utils_light.o $(SYSCALLS_LIGHT_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer.tns: ndless_installer.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader.exe $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	7z a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

ndless_resources.elf: install.o ints.o syscalls.o utils.o $(SYSCALLS_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

ndless_resources.tns: ndless_resources.elf
	$(OBJCOPY) -O binary $< $(DISTDIR)/$@

clean: cleanbin cleanapp
cleanapp:
	@# -: may fail because nspire_emu has a lock on it
	-rm -rf $(DISTDIR)

cleanbin:
	rm -rf *.o *.bin *.elf *.xml *.zip
