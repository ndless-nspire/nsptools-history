#!/bin/sh
# Cygwin required to run this file: http://www.cygwin.com/
if [ $# -ne 1 ]; then
	echo "Usage: assemble.sh <file.s>"
	exit 0
fi
infile="$1"
infile_noext="${infile%.s}"
if [ ! -e "$infile" ]; then
	echo "'$infile' not found"
	exit 1
fi
rm -f "$infile_noext".o "$infile_noext".elf "$infile_noext".bin
arm-elf-as -mcpu=arm7tdmi -o "$infile_noext".o "$infile_noext".s
arm-elf-ld -o "$infile_noext".elf "$infile_noext".o
#echo "Objdump:"
#arm-elf-objdump -d "$infile_noext".elf
arm-elf-objcopy -O binary "$infile_noext".elf "$infile_noext".bin
rm -f "$infile_noext".o "$infile_noext".elf
mv demo.tns ../demo.tns