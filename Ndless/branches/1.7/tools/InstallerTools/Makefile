include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init java$(EXEEXT)

init:
	@mkdir -p ../../bin

java$(EXEEXT): javarunner.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	@cd ../../bin && rm -rf java$(EXEEXT)
