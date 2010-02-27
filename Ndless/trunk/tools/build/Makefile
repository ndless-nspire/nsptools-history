CC=gcc
CFLAGS=-W -Wall
SCRIPTS=nspire-gcc.bat nspire-gcc
vpath %.exe ../../bin

all: init scripts

init:
	@mkdir -p ../../bin

scripts:
	@cp $(SCRIPTS) ../../bin

clean:
	@cd ../../bin && rm -rf $(SCRIPTS)
