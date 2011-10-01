CC = gcc
CFLAGS = -W -Wall

all: idcmigrate.exe

idcmigrate.exe: idcmigrate.o
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.exe
