SUBDIRS := tests
AS := nspire-as
GCC := nspire-gcc
GCCFLAGS := -Os -nostdlib -Wall -W -Wno-strict-aliasing
LD := nspire-ld
LDFLAGS := -nostdlib
OBJCOPY := "$(shell (which arm-elf-objcopy arm-none-eabi-objcopy arm-linux-gnueabi-objcopy | head -1) 2>/dev/null)"
DISTDIR := ../calcbin
vpath %.tno $(DISTDIR)
vpath %.tnc $(DISTDIR)
vpath %.tco $(DISTDIR)
vpath %.tcc $(DISTDIR)
vpath %.tns $(DISTDIR)

.DEFAULT_GOAL := all

ostargets.mk: mkostargets.sh
	./mkostargets.sh > ostargets.mk

include ostargets.mk

all: $(INSTALLER_FILES) static ndless_resources.tns subdirs
res: ndless_resources.tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@

static:
	@mkdir -p $(DISTDIR)
	@cp ndless.cfg.tns $(DISTDIR)

# Build with the light and stage1 switches (temporarily added to $GCCFLAGS), and in thumb mode
# because of the size limit of the installer
ints_light_thumb.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light_thumb.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
# utils.c contains C-preprocessor logic based on OS_VERSION. Build an .o for each version.
utils_light_thumb_%.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
# Prevent automatic deletion of these files considered as intermediate because not explicitely named as a target 
.PRECIOUS: utils_light_thumb_%.o

# Explicit dependencies strangely required for the target to avoid being rebuilt when not really needed
stage1.o: stage1.c
stage0.o: stage0.S

# To be used as a target-specific value for each ndless_installer_*.elf.
# Use with $(call GCCFLAGS_INSTALLER, <os_version>)
# OS_VERSION has its '.' replaced by '_' by this function.
# OS_VERSION_INT only contains digits for easier case testing with the C preprocessing.
GCCFLAGS_INSTALLER =-D _NDLS_LIGHT -D STAGE1 -mthumb-interwork -mthumb -D OS_VERSION=$(subst .,_,$(1)) -D OS_VERSION_INT=$(subst .,,$(1)) -D MODEL=$(MODEL)

# Required for $$*. Caution, applies to all the following targets.
.SECONDEXPANSION:
# stage0.o is the entry point: must be first in the list, and linked with --light-startup
ndless_installer-%-ncas.elf: stage0.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_ncas-$$*.o syscalls-light_ncas-%.o
	$(LD) --light-startup-no-header $(LDFLAGS) $^ -o $@
ndless_installer-%-cas.elf: stage0.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_cas-$$*.o syscalls-light_cas-%.o
	$(LD) --light-startup-no-header $(LDFLAGS) $^ -o $@
ndless_installer-%-ncascx.elf: stage0.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_ncascx-$$*.o syscalls-light_ncascx-%.o
	$(LD) --light-startup-no-header $(LDFLAGS) $^ -o $@
ndless_installer-%-cascx.elf: stage0.o stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_cascx-$$*.o syscalls-light_cascx-%.o
	$(LD) --light-startup-no-header $(LDFLAGS) $^ -o $@

ndless_installer-%.bin: ndless_installer-%.elf
	$(OBJCOPY) -O binary $< $@

ndless_installer-%.tno: ndless_installer-%-ncas.bin tno_template.bin
	mkdir -p $(DISTDIR)
	MakeTNO $< $(DISTDIR)/$@ $(OS_NAME) tno_template.bin  86

ndless_installer-%.tnc: ndless_installer-%-cas.bin tno_template.bin
	mkdir -p $(DISTDIR)
	MakeTNO $< $(DISTDIR)/$@ $(OS_NAME) tno_template.bin 86

ndless_installer-%.tco: ndless_installer-%-ncascx.bin tno_template.bin
	mkdir -p $(DISTDIR)
	MakeTNO $< $(DISTDIR)/$@ $(OS_NAME)  tno_template.bin 86

ndless_installer-%.tcc:ndless_installer-%-cascx.bin tno_template.bin
	mkdir -p $(DISTDIR)
	MakeTNO $< $(DISTDIR)/$@ $(OS_NAME) tno_template.bin  86

ndless_installer-bin-%.o: ndless_installer-%.bin tno_template_installed.bin 
	MakeTNO $< $(@:.o=.tmp.o) $* tno_template_installed.bin  0
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $(@:.o=.tmp.o) $@
#	@rm $(@:.o=.tmp.o)

.PRECIOUS: ndless_installer-%.bin

SYSCALLS_LIGHT_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, $(MODELS), syscalls-light_$(model)-$(os_version).o))
# Prevent automatic deletion of these files considered as intermediate because not explicitely named as a target 
.PRECIOUS: $(SYSCALLS_LIGHT_OBJS)
SYSCALLS_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, $(MODELS), syscalls_$(model)-$(os_version).o))
INSTALLER_BIN_OBJS := $(foreach os_version, $(OS_VERSIONS), $(foreach model, $(MODELS), ndless_installer-bin-$(os_version)-$(model).o))
CORE_OBJS := install.o ploaderhook.o ints.o syscalls.o utils.o emu.o config.o $(SYSCALLS_OBJS) $(INSTALLER_BIN_OBJS)
RES_OBJS := $(CORE_OBJS)

# Build with the light switch (temporarily added to $GCCFLAGS)
# (no thumb mode because HOOK_() isn't compatible with this mode)
ints_light.o: ints.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
syscalls_light.o: syscalls.c
	$(GCC) $(GCCFLAGS) -c $< -o $@
utils_light.o: utils.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

ndless_core.elf: $(CORE_OBJS)
	$(LD) --light-startup $(LDFLAGS) $^ -o $@

ndless_resources.tns: ndless_core.elf
	$(OBJCOPY) -O binary $< $(DISTDIR)/$@

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

clean: cleanbin cleanapp cleansubdirs
cleanapp:
	rm -rf $(DISTDIR)/ndless_installer*.t?? $(DISTDIR)/ndless_resources.tns $(DISTDIR)/ndless.cfg.tns
cleaninst:
	rm -rf ndless_installer.elf $(INST_OBJS)
cleanres:
	rm -rf ndless_core.elf $(RES_OBJS)

cleanbin:
	rm -rf *.o *.elf *.tno *.tnc ndless_installer-*.bin
	
cleansubdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make clean) || exit 1; done
