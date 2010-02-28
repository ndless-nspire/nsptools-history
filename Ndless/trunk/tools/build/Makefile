CC=gcc
CFLAGS=-W -Wall
SCRIPTS=nspire-gcc.bat nspire-ld.bat nspire-gcc nspire-ld
vpath ../../bin

all: init scripts

init:
	@mkdir -p ../../bin

scripts:
	@cp $(SCRIPTS) ../../bin

clean:
	@cd ../../bin && rm -rf $(SCRIPTS)
