SUBDIRS = tests
AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os -nostdlib
GCCFLAGS_DEFAULT := $(GCCFLAGS)
LD = nspire-ld
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
7Z := "$(shell which 7z 2>/dev/null)"
ifeq (${7Z},"")
	7Z := 7za
endif
DISTDIR = ../calcbin
# OS-specific
SYSCALLS_LIGHT_OBJS = syscalls-light_ncas-2.0.1.o syscalls-light_cas-2.0.1.o
SYSCALLS_OBJS = syscalls_ncas-1.7.o syscalls_cas-1.7.o syscalls_ncas-2.0.1.o syscalls_cas-2.0.1.o
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

# Build with the light and stage1 switches (temporarily added to $GCCFLAGS), and in thumb mode
# because of the 1kb limit of ndless_installer.tns
ints_light_thumb.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light_thumb.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
utils_light_thumb.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
ndless_installer.elf: GCCFLAGS:=$(GCCFLAGS_DEFAULT) -D _NDLS_LIGHT -D STAGE1 -mthumb-interwork -mthumb
# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup
INST_OBJS = bootstrapper.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb.o $(SYSCALLS_LIGHT_OBJS)
ndless_installer.elf: $(INST_OBJS)
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer.tns: ndless_installer.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	$(7Z) a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

STAGE2_OBJS = stage2.o ints_light.o syscalls_light.o utils_light.o $(SYSCALLS_LIGHT_OBJS)
CORE_OBJS = install.o ploaderhook.o ints.o syscalls.o utils.o emu.o $(SYSCALLS_OBJS)
RES_OBJS = $(STAGE2_OBJS) $(CORE_OBJS)

# Build with the light switch (temporarily added to $GCCFLAGS)
# because the "stage2" part of ndless_resources.tns is run from a small temporary buffer
# (no thumb mode because HOOK_() isn't compatible with this mode)
ints_light.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
utils_light.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
ndless_stage2.elf: GCCFLAGS:=$(GCCFLAGS_DEFAULT) -D _NDLS_LIGHT
ndless_stage2.elf: $(STAGE2_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

ndless_stage2.bin: ndless_stage2.elf
	$(OBJCOPY) -O binary $< $@

ndless_core.elf: $(CORE_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

ndless_core.bin: ndless_core.elf
	$(OBJCOPY) -O binary $< $@

# ndless_installer.tns loads ndless_stage2.
# ndless_stage2 loads ndless_core.
# ndless_stage2 and ndless_core are both packed into ndless_resources.tns:
#   <stage2_size(word)> <stage2> <core>
ndless_resources.tns: ndless_stage2.bin ndless_core.bin
	@# Write the stage2 size as a little-endian word
	stage2_hexsize=`wc -c ndless_stage2.bin | cut -f1 -d' ' | xargs  printf '%04x'` \
	&& test -n "$$stage2_hexsize" \
	&& echo -e -n "\x$${stage2_hexsize:2:2}\x$${stage2_hexsize:0:2}\x00\x00" > $(DISTDIR)/$@
	cat ndless_stage2.bin ndless_core.bin >> $(DISTDIR)/$@

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

clean: cleanbin cleanapp cleansubdirs
cleanapp:
	rm -rf $(DISTDIR)/ndless_installer.tns $(DISTDIR)/ndless_resources.tns
cleaninst:
	rm -rf ndless_installer.elf $(INST_OBJS)
cleanres:
	rm -rf ndless_stage2.elf ndless_core.elf $(RES_OBJS)

cleanbin:
	rm -rf *.o *.bin *.elf *.xml *.zip

cleansubdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make clean) || exit 1; done
