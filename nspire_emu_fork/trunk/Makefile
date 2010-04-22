OBJ = apb.o armloader.o cpu.o debug.o emu.o fastmem.o flash.o gdbstub.o gui.o link.o memory.o resource.o sha256.o shifts.o snippets.o translate.o usblink.o

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

armsnippets.o: armsnippets.S
	arm-elf-gcc -c -mcpu=arm7tdmi $< -o $@

snippets.o: armsnippets.o
	arm-elf-objcopy -O binary $< snippets.bin
	ld -r -b binary -o snippets.o snippets.bin
	rm snippets.bin
	objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents snippets.o snippets.o

%.o : %.c
	gcc $(FLAGS) -O2 -c $< -o $@

clean:
	rm -rf *.o *.bin emu.exe
