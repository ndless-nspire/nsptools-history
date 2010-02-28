include ../../Makefile.include

CC=gcc
CFLAGS = -W -Wall
vpath $(VPATH_EXEEXT) ../../bin

all: init LoaderWrapper$(EXEEXT)

init:
	@mkdir -p ../../bin

LoaderWrapper$(EXEEXT): LoaderWrapper.c
	$(CC) $(CFLAGS) $< -o ../../bin/$@

clean:
	@cd ../../bin && rm -rf LoaderWrapper$(EXEEXT) $(SCRIPTS)
