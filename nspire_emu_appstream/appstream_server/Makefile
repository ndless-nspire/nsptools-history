APPSTREAM_SDK_DIR=C:\XStxSdk
DISTDIR=../
vpath %.exe $(DISTDIR)
OBJ = AppStreamCalc.o AppStreamHostedApplication.o AppStreamServerManagerListener.o main.o

FLAGS = -W -Wall -I$(APPSTREAM_SDK_DIR)\include
LDFLAGS = -L$(APPSTREAM_SDK_DIR)\lib -lXStxServerLibraryShared -static-libgcc -static-libstdc++ -lshlwapi
CPPFLAGS =-std=c++0x

CC ?= gcc
CPP := g++
LD ?= ld

all : nspire_emu_appstream_server.exe

nspire_emu_appstream_server.exe : $(OBJ)
	$(CPP) $(FLAGS) $(OBJ) -o $(DISTDIR)/$@ -s $(LDFLAGS)

%.o : %.c
	$(CC) $(FLAGS) -Os -c $< -o $@

%.o : %.cpp
	$(CPP) $(FLAGS) $(CPPFLAGS) -Os -c $< -o $@

clean : 
	rm -f *.o $(DISTDIR)/nspire_emu_appstream_server.exe
