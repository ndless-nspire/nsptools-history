CC:=gcc
CFLAGS:=-W -Wall
LDFLAGS:= -lssl -lz
VPATH := minizip-1.1

OS ?= `uname -s`
ifeq ($(OS),Windows_NT)
  EXEEXT = .exe
endif

all: luna$(EXEEXT)

luna$(EXEEXT): luna.o zip.o ioapi.o
	gcc -o $@ $^ $(LDFLAGS)

dist: clean all
	mkdir -p dist/src
	rm -f *.o
	find . -maxdepth 1 ! -name 'luna$(EXEEXT)' -a ! -name dist -a ! -name . -exec cp -r {} dist/src \;
	cp luna$(EXEEXT) *.dll *.txt dist

clean:
	rm -rf *.o luna$(EXEEXT) dist
