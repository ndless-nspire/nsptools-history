AS = arm-elf-as
ASFLAGS = -mcpu=arm7tdmi
LD = arm-elf-ld
LDFLAGS = 
OBJCOPY = arm-elf-objcopy

all: loader hook demo

loader: loader.s components/files.s
	@rm -rf loader.bin
	$(AS) $(ASFLAGS) -o loader.o loader.s components/files.s
	$(LD) $(LDFLAGS) -Ttext 0x1800E15C -o loader.elf loader.o
	$(OBJCOPY) -O binary loader.elf loader.bin
	rm -f loader.o loader.elf
	../tools/LoaderWrapper/LoaderWrapper loader.bin ../res/loader.tns
	
hook:
	@rm -rf hook.bin
	$(AS) $(ASFLAGS) -o hook.o hook.s components/files.s
	$(LD) $(LDFLAGS) -o hook.elf hook.o
	$(OBJCOPY) -O binary hook.elf hook.bin
	rm -f hook.o hook.elf
	cp -f hook.bin ../res/hook.tns

demo:
	@rm -rf demo.bin
	$(AS) $(ASFLAGS) -o demo.o demo.s
	$(LD) $(LDFLAGS) -o demo.elf demo.o
	$(OBJCOPY) -O binary demo.elf demo.bin
	rm -f demo.o demo.elf
	cp -f demo.bin ../res/demo.tns

emulator:
	../tools/FlashEdit/FlashEdit '../res/loader.tns' '/Users/geogeo/Development/Calculatrice TI/Nspire/Emulator/011/Flash.bin' 0x611D60

.PHONY: cleanall

cleanall: clean

clean:
	@rm -rf loader.bin
	@rm -rf hook.bin
	@rm -rf demo.bin