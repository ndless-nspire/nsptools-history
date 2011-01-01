include Makefile.config
vpath = ..
OBJ = apb.o armloader.o asmcode.o cpu.o debug.o emu.o flash.o gdbstub.o gui.o interrupt.o lcd.o link.o memory.o mmu.o resource.o sha256.o snippets.o translate.o usb.o usblink.o

FLAGS := $(FLAGS) -W -Wall

all : nspire_emu.exe

nspire_emu.exe : $(OBJ)
	gcc $(FLAGS) $(OBJ) -o ../$@ -lgdi32 -lcomdlg32 -lwinmm -lws2_32

cpu.o : cpu.c
	gcc $(FLAGS) -O3 -c $< -o $@

resource.o : resource.rc id.h
	windres $< -o $@

sha256.o : sha256.c
	gcc $(FLAGS) -O3 -c $< -o $@

asmcode.o : asmcode.S
	gcc -c $< -o $@

armsnippets.o: armsnippets.S
	arm-none-eabi-gcc -c -mcpu=arm7tdmi $< -o $@

snippets.o: armsnippets.o
	arm-none-eabi-objcopy -O binary $< snippets.bin
	ld -r -b binary -o snippets.o snippets.bin
	rm snippets.bin
	objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents snippets.o snippets.o

%.o : %.c
	gcc $(FLAGS) -Os -c $< -o $@

clean : cleantmp
	rm -f ../nspire_emu.exe

cleantmp:
	rm -f *.o *.bin
