#!/bin/sh
if [ $# -ne 2 ]; then
	echo "Usage: assemble.sh <file.s> <nspire_hardware>"
	exit 0
fi
infile="$1"
infile_noext="${infile%.s}"
nspire_hardware="$2"
if [ ! -e "$infile" ]; then
	echo "'$infile' not found"
	exit 1
fi

arm-elf-gcc -mcpu=arm7tdmi -nostdlib -D GNU_AS -D ${nspire_hardware} -o "$infile_noext".o "$infile_noext".S
arm-elf-ld -o "$infile_noext".elf "$infile_noext".o
arm-elf-objcopy -O binary "$infile_noext".elf "$infile_noext".tns
rm -f "$infile_noext".o "$infile_noext".elf
