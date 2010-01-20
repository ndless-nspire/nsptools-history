SUBDIRS = tools java

all: subdirs arm

.PHONY: subdirs arm

arm:
	(cd arm; NSPIRE_HARDWARE=CAS make)
	(cd arm; NSPIRE_HARDWARE=NON_CAS make)

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all); done

dist: all
	mkdir -p dist/res
	cp java/dist/install-ndless.jar dist/res
	cp -r arm/res/* dist/res
	cp java/install-ndless.bat dist
	cp doc/ReadMe.txt dist
  
clean:
	@rm -rf dist
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
