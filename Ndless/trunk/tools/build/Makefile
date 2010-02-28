CC=gcc
CFLAGS=-W -Wall
SCRIPTS=nspire-gcc.bat nspire-gcc nspire-ld nspire-ld.bat
vpath %.exe ../../bin

all: init scripts

init:
	@mkdir -p ../../bin

scripts:
	@cp $(SCRIPTS) ../../bin

clean:
	@cd ../../bin && rm -rf $(SCRIPTS)
