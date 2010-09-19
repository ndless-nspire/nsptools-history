SUBDIRS = tests
AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os -nostdlib
LD = nspire-ld
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
7Z := "$(shell which 7z 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := 7za
endif
DISTDIR = ../calcbin
# OS-specific
SYSCALLS_LIGHT_OBJS = syscalls-light_ncas-1.7.o syscalls-light_cas-1.7.o
SYSCALLS_OBJS = syscalls_ncas-1.7.o syscalls_cas-1.7.o
vpath %.tns $(DISTDIR)

all: static ndless_installer.tns ndless_resources.tns subdirs
res: ndless_resources.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@


static:
	@mkdir -p $(DISTDIR)

# Build with the light switch (temporarily added to $GCCFLAGS), and in thumb mode,
# because of the 1kb limit of ndless_installer.tns
ints_light.o: ints.c
	$(GCC) $(GCCFLAGS) -mthumb -c $< -o $@
syscalls_light.o: syscalls.c
	$(GCC) $(GCCFLAGS) -mthumb -c $< -o $@
utils_light.o: utils.c
	$(GCC) $(GCCFLAGS) -mthumb -c $< -o $@

loader.o: GCCFLAGS:=$(GCCFLAGS) -D _NDLS_LIGHT -mthumb-interwork -mthumb
ndless_installer.elf: GCCFLAGS:=$(GCCFLAGS) -D _NDLS_LIGHT -mthumb-interwork
# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup 
ndless_installer.elf: bootstrapper.o loader.o ints_light.o syscalls_light.o utils_light.o $(SYSCALLS_LIGHT_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer.tns: ndless_installer.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	$(7Z) a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

RES_OBJS = install.o ints.o ploaderhook.o syscalls.o utils.o $(SYSCALLS_OBJS)
ndless_resources.elf: $(RES_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

ndless_resources.tns: ndless_resources.elf
	$(OBJCOPY) -O binary $< $(DISTDIR)/$@

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

clean: cleanbin cleanapp cleansubdirs
cleanapp:
	rm -rf $(DISTDIR)/ndless_installer.tns $(DISTDIR)/ndless_resources.tns
cleanres:
	rm -rf ndless_resources.elf $(RES_OBJS)

cleanbin:
	rm -rf *.o *.bin *.elf *.xml *.zip

cleansubdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make clean) || exit 1; done
