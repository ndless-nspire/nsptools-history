#!/bin/sh
# Migrate the symbols from the CX CAS v3.1 IDC to all other versions, and import them into the IDBs.

FROM_OS_VERSION=cascx-3.1.0
TO_OS_VERSIONS="ncas-3.1.0 cas-3.1.0 ncascx-3.1.0"

mkdir -p backup

echo "Backing up OS_${FROM_OS_VERSION}.idb" to backup/..."
cp OS_${FROM_OS_VERSION}.idb" backup

echo "Dumping source IDB OS_${FROM_OS_VERSION}.idb"
./idb2idc.sh OS_${FROM_OS_VERSION}.idb

for ver in $TO_OS_VERSIONS; do
	idcmigrate/idcmigrate phoenix-${FROM_OS_VERSION}.raw OS_${FROM_OS_VERSION}.idc phoenix-${ver}.raw idcma_temp.idc | grep -v Looking | grep -v Done
	if [ $? -ne 0 ]; then
		echo "Error returned by idcmigrate"
		exit 1
	fi
	
	#./idb2idc.sh OS_${ver}.idb
	
	echo "#include <idc.idc>" > out.idc
	echo "static main(void) {" >> out.idc
	cat idcma_temp.idc >> out.idc
#	echo 'SaveBase("", 0);' >> out.idc
#	echo "Exit(0);" >> out.idc
	echo "}" >> out.idc

	echo "Backing up OS_${ver}.idb to backup/..."
	cp OS_${ver}.idb backup
	# Doesn't work: fail if too many already known names
	#echo "Don't interrupt me! Writing OS_${ver}.idb..."
	#idag -A -Sout.idc OS_${ver}.idb | grep -v "Thank you"
	echo "Now load out.idc."
	idag OS_${ver}.idb | grep -v "Thank you"
	if [ $? -ne 0 ]; then
		echo "Error returned by idag"
		exit 1
	fi
	echo "OS_${ver}.idb saved."
done

rm idcma_temp.idc out.idc diff.idc
