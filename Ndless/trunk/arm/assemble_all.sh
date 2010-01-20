#!/bin/sh

echo Nspire CAS
echo '##########'
NSPIRE_HARDWARE=CAS make
echo

echo Nspire NON-CAS
echo '##############'
NSPIRE_HARDWARE=NON_CAS make
