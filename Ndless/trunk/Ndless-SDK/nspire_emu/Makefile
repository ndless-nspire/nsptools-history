OBJ = apb.o asmcode.o cpu.o des.o debug.o emu.o flash.o gui.o interrupt.o keypad.o lcd.o link.o memory.o mmu.o os-win32.o resource.o schedule.o sha256.o translate.o usb.o usblink.o

FLAGS = -W -Wall

emu.exe : $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $@ -lgdi32 -lcomdlg32 -lwinmm

nspire_emu.exe : $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $@ -lgdi32 -lcomdlg32 -lwinmm -s -Wl,--nxcompat

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
