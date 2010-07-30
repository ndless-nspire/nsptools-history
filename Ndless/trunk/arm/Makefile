include ../Makefile.config

SUBDIRS = demo

NSPIRE_HARDWARE ?= NON_CAS

AS = nspire-as
GCC = nspire-gcc
GCCFLAGS = -Os -D $(NSPIRE_HARDWARE) -nostdlib
LD = nspire-ld
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},)
	OBJCOPY := arm-none-eabi-objcopy
endif
SRC = components/files.S components/utils.S
DISTDIR = ../res/$(NSPIRE_HARDWARE)
vpath %.tns  ../res/$(NSPIRE_HARDWARE)
vpath %.S  components

ifeq ($(NSPIRE_HARDWARE),CAS)
	NS_EMU_STRING_RES_OFFSET=0x611D60
else
	NS_EMU_STRING_RES_OFFSET=0x691320
endif

all: static loader.tns hook.tns subdirs

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.S
	$(AS) $(GCCFLAGS) -c $<

%.elf: %.o
	$(LD) $(LDFLAGS) $^ -o $@

%.tns: %.elf
	$(OBJCOPY) -O binary $< $@
	mv  $@ $(DISTDIR)

loader.elf: loader.o files.o utils.o
hook.elf: hook.o files.o utils.o

loader.tns: loader.elf
	$(OBJCOPY) -O binary $< $(@:.tns=.bin)
	LoaderWrapper $(@:.tns=.bin) $@
	rm $(@:.tns=.bin)
	mv  $@ $(DISTDIR)

static:
	mkdir -p ../res/$(NSPIRE_HARDWARE)
	cp -f copier.tno ../res/copier.tno

emulator: loader
	FlashEdit ../res/$(NSPIRE_HARDWARE)/loader.tns '$(NS_EMU_FLASH_PATH_$(NSPIRE_HARDWARE))' $(NS_EMU_STRING_RES_OFFSET)

.PHONY: clean

clean: cleanbin cleanapp
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done

cleanapp:
	rm -rf ../res/$(NSPIRE_HARDWARE)

cleanbin:
	rm -rf loader.bin hook.bin *.o *.elf
