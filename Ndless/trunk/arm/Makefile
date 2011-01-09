SUBDIRS := tests
AS := nspire-as
GCC := nspire-gcc
GCCFLAGS := -Os -nostdlib -Wall -W -Wno-strict-aliasing
GCCFLAGS_DEFAULT := $(GCCFLAGS)
LD := nspire-ld
LDFLAGS := -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
7Z := "$(shell which 7z 2>/dev/null)"
ifeq (${7Z},"")
	7Z := 7za
endif
DISTDIR := ../calcbin
vpath %.tns $(DISTDIR)

# OS-specific
OS_VERSIONS := 1.7 2.0.1 2.1.0
# Since the installer is built specifically for an OS version, the version must be passed to the code.
# Unfortunately Makefile doesn't allow target-specific variable values to extract the stem from pattern rules,
# this list must be maintained manually.
# OS-specific
ndless_installer_os-1.7.elf: GCCFLAGS=$(call GCCFLAGS_INSTALLER,1.7)
ndless_installer_os-2.0.1.elf: GCCFLAGS=$(call GCCFLAGS_INSTALLER,2.0.1)
ndless_installer_os-2.1.0.elf: GCCFLAGS=$(call GCCFLAGS_INSTALLER,2.1.0)

# One installer for each OS version, because it is size constrained, and each OS version requires its own syscall table.
INSTALLER_TNS := $(foreach os_version, $(OS_VERSIONS), ndless_installer_os-$(os_version).tns)

all: static $(INSTALLER_TNS) ndless_resources.tns subdirs
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
# because of the 1kb limit of ndless_installer*.tns
ints_light_thumb.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light_thumb.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
# utils.c contains C-preprocessor logic based on OS_VERSION. Build an .o for each version.
utils_light_thumb_os-%.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
# Prevent automatic deletion of these files considered as intermediate because not explicitely named as a target 
.PRECIOUS: utils_light_thumb_os-%.o

# Explicit dependencies strangely required for the target to avoid being rebuilt when not really needed
bootstrapper.o: bootstrapper.S
stage1.o: stage1.c

# To be used as a target-specific value for each ndless_installer_*.elf.
# Use with $(call GCCFLAGS_INSTALLER, <os_version>)
# OS_VERSION has its '.' replaced by '_' by this function.
# OS_VERSION_INT only contains digits for easier case testing with the C preprocessing.
GCCFLAGS_INSTALLER = $(GCCFLAGS_DEFAULT) -D _NDLS_LIGHT -D STAGE1 -mthumb-interwork -mthumb -D OS_VERSION=$(subst .,_,$(1)) -D OS_VERSION_INT=$(subst .,,$(1))

# Required for $$*. Caution, applies to all the following targets.
.SECONDEXPANSION:
# bootstrapper.o is the entry point: must be first in the list, and linked with --no-startup
ndless_installer_os-%.elf: bootstrapper.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_os-$$*.o syscalls-light_ncas-%.o syscalls-light_cas-%.o
	$(LD) --no-startup $(LDFLAGS) $^ -o $@

ndless_installer_os-%.tns: ndless_installer_os-%.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	../bin/MakeLoader $(@:.tns=.bin) Document.xml
	rm -f $@ $(@:.tns=.zip)
	$(7Z) a $(@:.tns=.zip) Document.xml
	mv $(@:.tns=.zip) $(DISTDIR)/$@
	rm Document.xml

SYSCALLS_LIGHT_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, ncas cas, syscalls-light_$(model)-$(os_version).o))
STAGE2_OBJS := stage2.o ints_light.o syscalls_light.o utils_light.o $(SYSCALLS_LIGHT_OBJS)
SYSCALLS_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, ncas cas, syscalls_$(model)-$(os_version).o))
CORE_OBJS := install.o ploaderhook.o ints.o syscalls.o utils.o emu.o $(SYSCALLS_OBJS)
RES_OBJS := $(STAGE2_OBJS) $(CORE_OBJS)

# Build with the light switch (temporarily added to $GCCFLAGS)
# because the "stage2" part of ndless_resources.tns is run from a small temporary buffer
# (no thumb mode because HOOK_() isn't compatible with this mode)
ints_light.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
utils_light.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
ndless_stage2.elf: GCCFLAGS:=$(GCCFLAGS_DEFAULT) -D _NDLS_LIGHT -D STAGE2
ndless_stage2.elf: $(STAGE2_OBJS)
	$(LD) --light-startup  $(LDFLAGS) $^ -o $@

ndless_stage2.bin: ndless_stage2.elf
	$(OBJCOPY) -O binary $< $@

ndless_core.elf: $(CORE_OBJS)
	$(LD) --light-startup $(LDFLAGS) $^ -o $@

ndless_core.bin: ndless_core.elf
	$(OBJCOPY) -O binary $< $@

# ndless_installer*.tns loads ndless_stage2.
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
	rm -rf $(DISTDIR)/ndless_installer*.tns $(DISTDIR)/ndless_resources.tns
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
