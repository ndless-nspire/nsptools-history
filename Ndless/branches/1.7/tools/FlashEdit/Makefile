include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
SCRIPTS=install-ndless-nspire_emu.bat
vpath $(VPATH_EXEEXT) ../../bin

all: init scripts FlashEdit$(EXEEXT)

init:
	@mkdir -p ../../bin

scripts:
	@cp $(SCRIPTS) ../../bin

FlashEdit$(EXEEXT): FlashEdit.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	@cd ../../bin && rm -rf FlashEdit$(EXEEXT) $(SCRIPTS)
