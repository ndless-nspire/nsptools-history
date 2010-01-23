#!/bin/sh
# true for emu-specific setup (set sp, ...)
WITH_EMU_STARTUP=false
if [ $# -ne 1 ]; then
	echo "Usage: $0 <file.c>"
	exit 0
fi
cfile="$1"
cfile_noext="${cfile%.c}"

arm-elf-as.exe crt0.s -o crt0.o
arm-elf-gcc -T ldscript -Os -Wall -W -fpic -fno-merge-constants -nostdlib crt0.o "$cfile" -o "$cfile_noext".elf
if [ $? -ne 0 ]; then
	exit 1
fi
arm-elf-objdump -s "$cfile_noext".elf
#arm-elf-objdump -d "$cfile_noext".elf
arm-elf-objcopy -O binary "$cfile_noext".elf "$cfile_noext".bin

if [ $WITH_EMU_STARTUP = 'true' ]; then
	cat startup.bin "$cfile_noext".bin > "$cfile_noext".bin2
	mv "$cfile_noext".bin2 "$cfile_noext".bin
fi
