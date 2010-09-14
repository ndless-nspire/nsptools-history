include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init MakeLoader$(EXEEXT)

init:
	@mkdir -p ../../bin

MakeLoader$(EXEEXT): MakeLoader.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	@cd ../../bin && rm -rf MakeLoader$(EXEEXT)
