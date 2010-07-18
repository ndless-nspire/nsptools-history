CC=gcc
CFLAGS = -W -Wall

all: MakeLoader.exe

MakeLoader.exe: MakeLoader.c
	$(CC) $(CFLAGS) $< -o $@

loader:
	./MakeLoader.exe ../nleash.bin Document.xml
	rm -f ../nleash.tns nleash.zip
	7z a nleash.zip Document.xml
	mv nleash.zip nleash.tns

clean:
	rm -rf MakeLoader
