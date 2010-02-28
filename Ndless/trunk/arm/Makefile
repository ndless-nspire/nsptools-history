include ../Makefile.config

SUBDIRS = demo

NSPIRE_HARDWARE ?= NON_CAS

GCC = arm-elf-gcc
GCCFLAGS = -Os -nostdlib -D GNU_AS -D $(NSPIRE_HARDWARE)
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy
SRC = components/files.S components/utils.S

ifeq ($(NSPIRE_HARDWARE),CAS)
	NS_EMU_STRING_RES_OFFSET=0x611D60
else
	NS_EMU_STRING_RES_OFFSET=0x691320
endif

all: static loader hook demo subdirs

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all); done

loader: loader.S $(SRC)	
	@rm -rf $@.bin
	$(GCC) $(GCCFLAGS) -Ttext 0x1800E15C $< $(SRC) -o $@.o
	$(LD) $(LDFLAGS) -Ttext 0x1800E15C -o $@.elf $@.o
	$(OBJCOPY) -O binary $@.elf $@.bin
	rm -f $@.o $@.elf
	LoaderWrapper $@.bin ../res/$(NSPIRE_HARDWARE)/$@.tns
	
hook: hook.S $(SRC)
	@rm -rf $@.bin
	$(GCC) $(GCCFLAGS) $< $(SRC) -o $@.o
	$(LD) $(LDFLAGS) -o $@.elf $@.o
	$(OBJCOPY) -O binary $@.elf $@.bin
	rm -f $@.o $@.elf
	cp -f $@.bin ../res/$(NSPIRE_HARDWARE)/$@.tns

.PHONY: demo
demo:
	cd demo; make clean; make all
	
static:
	mkdir -p ../res/$(NSPIRE_HARDWARE)
	cp -f copier.tno ../res/copier.tno

emulator: loader
	FlashEdit ../res/$(NSPIRE_HARDWARE)/loader.tns '$(NS_EMU_FLASH_PATH_$(NSPIRE_HARDWARE))' $(NS_EMU_STRING_RES_OFFSET)

.PHONY: clean

clean: cleanbin cleanapp
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done

cleanapp:
	@rm -rf ../res/CAS ../res/NON_CAS

cleanbin:
	@rm -rf loader.bin hook.bin
