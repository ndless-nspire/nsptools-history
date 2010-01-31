include ../Makefile.config

NSPIRE_HARDWARE ?= NON_CAS

AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
GCC = arm-elf-gcc
GCCFLAGS = -mcpu=arm7tdmi
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

ifeq ($(NSPIRE_HARDWARE),CAS)
	NS_EMU_STRING_RES_OFFSET=0x5DC990
else
	NS_EMU_STRING_RES_OFFSET=0x613830
endif

all: loader hook demo static

loader: loader.S components/files.s	
	@rm -rf loader.bin
	$(GCC) $(GCCFLAGS) -E loader.S -o tmp_loader.S -D $(NSPIRE_HARDWARE)
	$(AS) $(ASFLAGS) -o loader.o tmp_loader.s components/files.s components/utils.s
	$(LD) $(LDFLAGS) -Ttext 0x1800E15C -o loader.elf loader.o
	$(OBJCOPY) -O binary loader.elf loader.bin
	rm -f tmp_loader.S loader.o loader.elf
	mkdir -p res/$(NSPIRE_HARDWARE)
	../tools/LoaderWrapper/LoaderWrapper loader.bin res/$(NSPIRE_HARDWARE)/loader.tns
	
hook: hook.S components/files.s
	@rm -rf hook.bin
	$(GCC) $(GCCFLAGS) -E hook.S -o tmp_hook.S -D $(NSPIRE_HARDWARE)
	$(AS) $(ASFLAGS) -o hook.o tmp_hook.S components/files.S
	$(LD) $(LDFLAGS) -o hook.elf hook.o
	$(OBJCOPY) -O binary hook.elf hook.bin
	rm -f tmp_hook.S hook.o hook.elf
	mkdir -p res/$(NSPIRE_HARDWARE)
	cp -f hook.bin res/$(NSPIRE_HARDWARE)/hook.tns

demo: demo.S
	@rm -rf demo.bin
	$(GCC) $(GCCFLAGS) -E demo.S -o tmp_demo.S -D $(NSPIRE_HARDWARE)
	$(AS) $(ASFLAGS) -o demo.o tmp_demo.S
	$(LD) $(LDFLAGS) -o demo.elf demo.o
	$(OBJCOPY) -O binary demo.elf demo.bin
	rm -f tmp_demo.S demo.o demo.elf
	mkdir -p res/$(NSPIRE_HARDWARE)
	cp -f demo.bin res/$(NSPIRE_HARDWARE)/demo.tns

static:
	cp -f copier.tno res

emulator: loader
	../tools/FlashEdit/FlashEdit res/$(NSPIRE_HARDWARE)/loader.tns '${NS_EMU_FLASH_PATH_${NSPIRE_HARDWARE}}' ${NS_EMU_STRING_RES_OFFSET}
.PHONY: clean

clean: cleanbin cleanapp

cleanapp:
	@rm -rf res

cleanbin:
	@rm -rf loader.bin hook.bin demo.bin
