SUBDIRS = hella hello particles
DISTDIR = ../calcbin/samples

all: doc
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

doc:
	@cp ReadMe.txt $(DISTDIR)
  
clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
	rm $(DISTDIR)/ReadMe.txt
