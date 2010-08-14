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
SYSCALLS_OBJS=syscalls_ncas-1.7.o syscalls_cas-1.7.o
vpath %.tns  ../res/$(NSPIRE_HARDWARE)

all: static ndless_installer.tns ndless_resources.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@


static:
	@mkdir -p ../res/$(NSPIRE_HARDWARE)

# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup 
ndless_installer.elf: bootstrapper.o loader.o ints.o syscalls.o utils.o $(SYSCALLS_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer.tns: ndless_installer.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader.exe $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	7z a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

ndless_resources.tns:
	

.PHONY: clean
clean: cleanbin cleanapp
cleanapp:
	rm -rf ../res/$(NSPIRE_HARDWARE)

cleanbin:
	rm -rf *.bin *.o *.elf *.xml *.zip
