include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init MakeTNO$(EXEEXT)

init:
	@mkdir -p ../../bin

MakeTNO$(EXEEXT): MakeTNO.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	 rm -f ../../bin/MakeTNO$(EXEEXT)
