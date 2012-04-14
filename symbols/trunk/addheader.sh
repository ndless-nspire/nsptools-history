#!/bin/sh
if [ $# -ne 1 ]; then
	echo "Usage: addheader.sh <raw.idc>"
	echo "Writes it to out.idc"
	exit 1
fi
echo "#include <idc.idc>" > out.idc
echo "static main(void) {" >> out.idc
cat $1 >> out.idc
echo "}" >> out.idc
