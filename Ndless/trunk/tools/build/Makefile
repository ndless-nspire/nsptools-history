CC=gcc
CFLAGS=-W -Wall
SCRIPTS=nspire-gcc nspire-ld nspire-ld-bflt nspire-as nspire-tools Makefile.tpl
vpath ../../Ndless-SDK/ndless/bin

all: init scripts

init:
	@mkdir -p ../../Ndless-SDK/ndless/bin

scripts:
	cp $(SCRIPTS) ../../Ndless-SDK/ndless/bin
	chmod +x ../../Ndless-SDK/ndless/bin/nspire-*

clean:
	cd ../../Ndless-SDK/ndless/bin && rm -f $(SCRIPTS) || :
