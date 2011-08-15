include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init encode85$(EXEEXT)

init:
	@mkdir -p ../../bin

encode85$(EXEEXT): encode85.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	 rm -f ../../bin/encode85$(EXEEXT)
