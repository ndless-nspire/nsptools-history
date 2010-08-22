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
# Shared between the installer and the resources: must be always rebuilt, because of the different compile options
SHARED_OBJS=ints.o syscalls.o utils.o
# OS-specific
SYSCALLS_LIGHT_OBJS=syscalls-light_ncas-1.7.o syscalls-light_cas-1.7.o
SYSCALLS_OBJS=syscalls_ncas-1.7.o syscalls_cas-1.7.o
vpath %.tns  ../res/$(NSPIRE_HARDWARE)

all: static
	rm -f $(SHARED_OBJS)
	make ndless_installer.tns
	rm -f $(SHARED_OBJS)
	make ndless_resources.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@


static:
	@mkdir -p ../res/$(NSPIRE_HARDWARE)

ndless_installer.elf: GCCFLAGS:=$(GCCFLAGS) -D _SYSCALLS_LIGHT
# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup 
ndless_installer.elf: bootstrapper.o loader.o $(SHARED_OBJS) $(SYSCALLS_LIGHT_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer.tns: ndless_installer.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader.exe $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	7z a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

ndless_resources.elf: $(SHARED_OBJS) install.o $(SYSCALLS_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_resources.tns: ndless_resources.elf
	$(OBJCOPY) -O binary $< $@

clean: cleanbin cleanapp
cleanapp:
	@# -: may fail because nspire_emu has a lock on it
	-rm -rf ../res/$(NSPIRE_HARDWARE)

cleanbin:
	rm -rf *.o *.bin *.elf *.xml *.zip
