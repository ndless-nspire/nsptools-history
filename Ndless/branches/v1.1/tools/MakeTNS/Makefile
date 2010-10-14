include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init MakeTNS$(EXEEXT)

init:
	@mkdir -p ../../bin

MakeTNS$(EXEEXT): MakeTNS.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	@cd ../../bin && rm -rf MakeTNS$(EXEEXT) $(SCRIPTS)
