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

dist: all
	rm -rf dist
	mkdir dist
	cp -r res dist
	cp java/dist/install-ndless.jar dist/res
	cp java/install-ndless.bat dist
	cp doc/ReadMe.txt dist
	mkdir dist/tools
	cp -r tools/FlashEdit tools/MakeTNS dist/tools
	(cd arm; make clean)
	cp -r arm dist
	@# exclude some resources we don't want to distribute
	find dist -name drawString.s -o -name Font8X.bin | xargs rm -rf
	find dist -name .svn | xargs rm -rf

clean:
	rm -rf dist
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
