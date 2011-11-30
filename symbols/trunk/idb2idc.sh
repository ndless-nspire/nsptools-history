#!/bin/sh
# Convert all the files in the current folder
[ -n "$1" -a -f "$1" ] && idbs="$1" || idbs="`ls *.idb`"
#idbs=OS_ncas-3.1.0.idb
for idb in $idbs; do
	idc=${idb%%.idb}.idc
	echo "$idc..."
	idag -Sidb2idc.idc -A $idb 
	./lighten_idc.sh < temp.idc > $idc
done
rm temp.idc idb2idc.idc
