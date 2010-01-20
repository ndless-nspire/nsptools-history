AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
GCC = arm-elf-gcc
GCCFLAGS = -mcpu=arm7tdmi
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

all: loader hook demo

loader: loader.S components/files.S	
	@rm -rf loader.bin
	$(GCC) $(GCCFLAGS) -E loader.S -o tmp_loader.S -D $(NSPIRE_HARDWARE)
	$(AS) $(ASFLAGS) -o loader.o tmp_loader.S components/files.S
	$(LD) $(LDFLAGS) -Ttext 0x1800E15C -o loader.elf loader.o
	$(OBJCOPY) -O binary loader.elf loader.bin
	rm -f tmp_loader.S loader.o loader.elf
	mkdir -p res/$(NSPIRE_HARDWARE)
	../tools/LoaderWrapper/LoaderWrapper loader.bin res/$(NSPIRE_HARDWARE)/loader.tns
	
hook: hook.S components/files.S
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

emulator:
	../tools/FlashEdit/FlashEdit ../res/$(NSPIRE_HARDWARE)/loader.tns '/Users/geogeo/Development/Calculatrice TI/Nspire/Emulator/011/Flash.bin' 0x611D60

.PHONY: clean

clean: cleanbin cleanapp

cleanapp:
	@rm -rf res

cleanbin:
	@rm -rf loader.bin hook.bin demo.bin
