include Makefile.config
vpath = ..
OBJ = apb.o asmcode.o cpu.o debug.o emu.o flash.o gui.o interrupt.o lcd.o link.o memory.o mmu.o resource.o sha256.o translate.o usblink.o

FLAGS := $(FLAGS) -W -Wall

nspire_emu.exe : $(OBJ)
	gcc $(FLAGS) $(OBJ) -o ../$@ -lgdi32 -lcomdlg32 -lwinmm

cpu.o : cpu.c
	gcc $(FLAGS) -O3 -c $< -o $@

resource.o : resource.rc id.h
	windres $< -o $@

sha256.o : sha256.c
	gcc $(FLAGS) -O3 -c $< -o $@

asmcode.o : asmcode.S
	gcc -c $< -o $@

%.o : %.c
	gcc $(FLAGS) -Os -c $< -o $@

clean : cleantmp
	rm -f ../nspire_emu.exe

cleantmp:
	rm -f *.o
