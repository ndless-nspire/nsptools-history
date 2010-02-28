SUBDIRS = build FlashEdit LoaderWrapper MakeTNS ../system

all:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all); done
  
install:
	mkdir -p /usr/local/nspire
	cp -fR ../bin /usr/local/nspire
	cp -fR ../include /usr/local/nspire
	cp -fR ../system /usr/local/nspire
	ln -sfv /usr/local/nspire/bin/nspire-gcc /usr/local/bin
	ln -sfv /usr/local/nspire/bin/nspire-ld /usr/local/bin
	ln -sfv /usr/local/nspire/bin/FlashEdit /usr/local/bin
	ln -sfv /usr/local/nspire/bin/MakeTNS /usr/local/bin 
	ln -sfv /usr/local/nspire/bin/LoaderWrapper /usr/local/bin

uninstall:
	rm -f /usr/local/bin/nspire-gcc
	rm -f /usr/local/bin/nspire-ld
	rm -f /usr/local/bin/FlashEdit
	rm -f /usr/local/bin/MakeTNS
	rm -f /usr/local/bin/LoaderWrapper
	rm -Rf /usr/local/nspire

clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
	rm -rf ../bin
