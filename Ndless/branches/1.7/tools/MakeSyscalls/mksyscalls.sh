#!/bin/sh
# The caller must set the environment variable IDC_FILES_DIR

# Parse syscalls.h and idc files for each OS version, and produce syscalls_x.x.c

if [ $# -eq 0 ]; then
	echo "Usage: $0 <OS_version_number>*"
	echo "<OS_version_number> is (ncas|cas)-x.y[.z]"
	echo "The IDC files must be named OS_<version_number>.idc"
	exit 1
fi

INCLUDEDIR="../../include"
ARMDIR="../../arm"
syscallfile="$INCLUDEDIR/syscalls.h"
syscallh_first_value_line=$((`grep -n START_OF_LIST "$syscallfile" | cut -d':' -f1` + 1))
syscallh_last_value_line=$((`grep -n  END_OF_LIST   "$syscallfile" | cut -d':' -f1` - 1))

for os_version in "$@"; do
	idcname="OS_${os_version}.idc"
	idcfile="$IDC_FILES_DIR/$idcname"
	outfile="$ARMDIR/syscalls_${os_version}.c"
	echo "Generating syscalls_${os_version}.c..."
	echo "/* Each entry matches a symbol in syscalls.h. This file is generated by `basename $0`. */" > "$outfile"
	array_name="syscalls_"`echo $os_version | sed 's/[.-]/_/g'` # replace reserved characters with '_'
	echo "unsigned $array_name[] = {" >> "$outfile"
	syscallh_linenum=$syscallh_first_value_line
	while [ $syscallh_linenum -le $syscallh_last_value_line ]; do
		echo -n '.'
		scallh_line=`head -$syscallh_linenum "$syscallfile" | tail -1`
		syscall_name=`echo "$scallh_line" | sed 's/.\+\?e_\(\w\+\).*/\1/'`
		idcline=`grep \"$syscall_name\" "$idcfile" | grep MakeName`
		if [ $? -ne 0 ]; then
			echo -e "\nWARNING: symbol '$syscall_name' of 'syscall.h' not found in '$idcname'"
			syscall_addr=0x0
		else
			syscall_addr=`echo "$idcline" | sed 's/.*\(0X[0-9A-F]\+\),.*/\1/g'`
		fi
		if [ $syscallh_linenum -eq $syscallh_first_value_line ]; then
			echo -en "\t  " >> "$outfile"
		else
			echo -en "\t, " >> "$outfile"
		fi
		echo "$syscall_addr" >> "$outfile"
		syscallh_linenum=$((syscallh_linenum + 1))
	done
	echo "};" >> "$outfile"
	echo ""
done
