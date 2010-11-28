CC=gcc
CFLAGS=-W -Wall
SCRIPTS=nspire-gcc nspire-ld  nspire-as
vpath ../../bin

all: init scripts

init:
	@mkdir -p ../../bin

scripts:
	cp $(SCRIPTS) ../../bin

clean:
	cd ../../bin && rm -f $(SCRIPTS) || :
