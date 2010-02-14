SUBDIRS = tools java

all: subdirs arm

.PHONY: subdirs arm

arm:
	(cd arm; make NSPIRE_HARDWARE=CAS)
	(cd arm; make NSPIRE_HARDWARE=NON_CAS)

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all); done

# Incremental binary dist for development
distbin: all
	mkdir -p dist
	cp -r res dist
	cp install-ndless.bat dist
	cp doc/ReadMe.txt dist
	rm -rf dist/tools
	mkdir -p dist/tools
	cp -r tools/FlashEdit tools/MakeTNS dist/tools
	find dist -name .svn | xargs rm -rf

# Dist with cleanup, binary and source
dist: cleandist distsrc distbin
	find dist -name .svn | xargs rm -rf

distsrc: clean
	mkdir -p dist/src
	cp -r arm dist/src
	@# exclude some resources we don't want to distribute
	find dist -name drawString.s -o -name Font8X.bin | xargs rm -rf

cleandist:
	rm -rf dist

clean: cleandist
	rm -rf res
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
