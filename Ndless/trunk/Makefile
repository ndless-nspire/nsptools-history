SUBDIRS = tools java system
SUBDIR_TOOLS = tools system
SUBDIRSCLEAN = $(SUBDIRS) arm

all: subdirs arm
all_tools: subdirs_tools

.PHONY: subdirs arm

arm:
	(cd arm && make clean && make NSPIRE_HARDWARE=CAS)
	(cd arm && make clean && make NSPIRE_HARDWARE=NON_CAS)

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

subdirs_tools:
	@for i in $(SUBDIR_TOOLS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

# Incremental binary dist for development
distbin: all
	mkdir -p dist
	cp -r bin include res system dist
	cp install-ndless.bat dist
	cp "Mozilla-Public-License-v1.1.html" doc/ReadMe.txt doc/ndless-particle-demo.gif dist
	find dist -name .svn | xargs rm -rf

# Dist with cleanup, binary and source
dist: cleandist distsrc distbin
	find dist -name .svn | xargs rm -rf

distsrc: clean
	mkdir -p dist/src
	cp -r `ls | grep -v dist` dist/src
	@# exclude some resources we don't want to distribute
	find dist -name drawString.s -o -name Font8X.bin -o -name build_config.properties \
	  -o -name proguard -o -name Makefile.config -o -wholename 'dist/src/java/bin/*' | xargs rm -rf

install: all_tools
	mkdir -p /usr/local/nspire
	cp -fR bin /usr/local/nspire
	cp -fR include /usr/local/nspire
	cp -fR system /usr/local/nspire
	ln -sfv /usr/local/nspire/bin/nspire-as /usr/local/bin
	ln -sfv /usr/local/nspire/bin/nspire-gcc /usr/local/bin
	ln -sfv /usr/local/nspire/bin/nspire-ld /usr/local/bin
	ln -sfv /usr/local/nspire/bin/FlashEdit /usr/local/bin
	ln -sfv /usr/local/nspire/bin/MakeTNS /usr/local/bin 
	ln -sfv /usr/local/nspire/bin/LoaderWrapper /usr/local/bin

uninstall:
	rm -f /usr/local/bin/nspire-as
	rm -f /usr/local/bin/nspire-gcc
	rm -f /usr/local/bin/nspire-ld
	rm -f /usr/local/bin/FlashEdit
	rm -f /usr/local/bin/MakeTNS
	rm -f /usr/local/bin/LoaderWrapper
	rm -Rf /usr/local/nspire

cleandist:
	rm -rf dist

clean: cleandist
	rm -rf res
	@for i in $(SUBDIRSCLEAN); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
