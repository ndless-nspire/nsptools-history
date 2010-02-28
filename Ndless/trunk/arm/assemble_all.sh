#!/bin/sh

echo Nspire CAS
echo '##########'
make clean
NSPIRE_HARDWARE=CAS make
echo

echo Nspire NON-CAS
echo '##############'
make clean
NSPIRE_HARDWARE=NON_CAS make
