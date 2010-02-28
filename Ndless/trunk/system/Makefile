OS ?= `uname -s`
ifeq ($(OS),Windows_NT)
  MAKEFILE_NAME = Makefile.win
else
  MAKEFILE_NAME = Makefile.unix
endif

all:
	make -f $(MAKEFILE_NAME) all
  
clean:
	make -f $(MAKEFILE_NAME) clean