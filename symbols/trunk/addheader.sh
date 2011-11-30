#!/bin/sh
echo "#include <idc.idc>" > out.idc
echo "static main(void) {" >> out.idc
cat $1 >> out.idc
echo "}" >> out.idc
