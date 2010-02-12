#!/bin/sh
# Cygwin required to run this file: http://www.cygwin.com/
if [ $# -ne 2 ]; then
	echo "Usage: assemble.sh <file.s> <nspire_hardware>"
	exit 0
fi
infile="$1"
infile_noext="${infile%.S}"
nspire_hardware="$2"
if [ ! -e "$infile" ]; then
	echo "'$infile' not found"
	exit 1
fi

arm-elf-gcc -mcpu=arm7tdmi -nostdlib -D GNU_AS -D ${nspire_hardware} -o "$infile_noext".o "$infile_noext".S ../components/files.S ../components/utils.S
arm-elf-ld -o "$infile_noext".elf "$infile_noext".o
arm-elf-objcopy -O binary "$infile_noext".elf "$infile_noext".bin
rm -f "$infile_noext".o "$infile_noext".elf
cp -f "$infile_noext".bin ../res/${nspire_hardware}/userfiles/"$infile_noext".tns
