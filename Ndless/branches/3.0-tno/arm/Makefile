SUBDIRS := tests
AS := nspire-as
GCC := nspire-gcc
GCCFLAGS := -Os -nostdlib -Wall -W -Wno-strict-aliasing
LD := nspire-ld
LDFLAGS := -nostdlib
OBJCOPY := "$(shell (which arm-elf-objcopy arm-none-eabi-objcopy arm-linux-gnueabi-objcopy | head -1) 2>/dev/null)"
7Z := "$(shell (which 7z 7za | head -1) 2>/dev/null)"
DISTDIR := ../calcbin
vpath %.tno $(DISTDIR)
vpath %.tns $(DISTDIR)

# OS-specific
# Comma-separated
OS_VERSIONS := 3.1.0
# Since the installer is built specifically for an OS version, the version must be passed to the code.
# Unfortunately Makefile doesn't allow target-specific variable values to extract the stem from pattern rules,
# this list must be maintained manually.
# OS-specific
ndless_installer_os-3.1.0.elf: GCCFLAGS+=$(call GCCFLAGS_INSTALLER,3.1.0)
ndless_installer_os-3.1.0.tno: OS_NAME=3.1.0-ncas

# One installer for each OS version, because it is size constrained, and each OS version requires its own syscall table.
INSTALLER_TNO := $(foreach os_version, $(OS_VERSIONS), ndless_installer_os-$(os_version).tno)

all: static $(INSTALLER_TNO) ndless_resources.tns subdirs
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
# because of the size limit of the installer
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
stage1.o: stage1.c

# To be used as a target-specific value for each ndless_installer_*.elf.
# Use with $(call GCCFLAGS_INSTALLER, <os_version>)
# OS_VERSION has its '.' replaced by '_' by this function.
# OS_VERSION_INT only contains digits for easier case testing with the C preprocessing.
GCCFLAGS_INSTALLER =-D _NDLS_LIGHT -D STAGE1 -mthumb-interwork -D OS_VERSION=$(subst .,_,$(1)) -D OS_VERSION_INT=$(subst .,,$(1))

# Required for $$*. Caution, applies to all the following targets.
.SECONDEXPANSION:
# stage1.o is the entry point: must be first in the list, and linked with --light-startup
ndless_installer_os-%.elf: stage1.o ints_light_thumb.o syscalls_light_thumb.o utils_light_thumb_os-$$*.o syscalls-light_ncas-%.o syscalls-light_cas-%.o
	$(LD) --light-startup $(LDFLAGS) $^ -o $@

ndless_installer_os-%.tno: ndless_installer_os-%.elf
	$(OBJCOPY) -O binary $< $(@:.tno=.bin)
	MakeTNO $(@:.tno=.bin) $(DISTDIR)/$@ $(OS_NAME)
	@rm -f $(@:.tno=.bin)

SYSCALLS_LIGHT_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, ncas cas, syscalls-light_$(model)-$(os_version).o))
# Prevent automatic deletion of these files considered as intermediate because not explicitely named as a target 
.PRECIOUS: $(SYSCALLS_LIGHT_OBJS)
SYSCALLS_OBJS :=  $(foreach os_version, $(OS_VERSIONS), $(foreach model, ncas cas, syscalls_$(model)-$(os_version).o))
CORE_OBJS := install.o ploaderhook.o ints.o syscalls.o utils.o emu.o $(SYSCALLS_OBJS)
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
	rm -rf $(DISTDIR)/ndless_installer*.tno $(DISTDIR)/ndless_resources.tns
cleaninst:
	rm -rf ndless_installer.elf $(INST_OBJS)
cleanres:
	rm -rf ndless_core.elf $(RES_OBJS)

cleanbin:
	rm -rf *.o *.bin *.elf *.tno
	
cleansubdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make clean) || exit 1; done
