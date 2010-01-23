OBJ = apb.o cpu.o debug.o emu.o fastmem.o flash.o gdbstub.o gui.o link.o memory.o resource.o sha256.o shifts.o translate.o usblink.o

FLAGS = -W -Wall

emu.exe : $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $@ -lgdi32 -lcomdlg32 -lwinmm -lws2_32

cpu.o : cpu.c
	gcc $(FLAGS) -O3 -c $< -o $@

fastmem.o : fastmem.c
	gcc $(FLAGS) -fomit-frame-pointer -O3 -c $< -o $@

resource.o : resource.rc id.h
	windres $< -o $@

sha256.o : sha256.c
	gcc $(FLAGS) -O3 -c $< -o $@

shifts.o : shifts.S
	gcc -c $< -o $@

%.o : %.c
	gcc $(FLAGS) -O2 -c $< -o $@

clean:
	rm -rf *.o emu.exe
