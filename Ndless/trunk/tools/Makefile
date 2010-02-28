SUBDIRS = build FlashEdit LoaderWrapper MakeTNS

all:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all); done
  
clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
	rm -rf ../bin
