SUBDIRS = colors hella hello particles luaext
DISTDIR = ../calcbin/samples

all: distdir doc
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

distdir:
	@mkdir -p $(DISTDIR)

doc: distdir
	@cp ReadMe.txt $(DISTDIR)
  
clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
	rm -f $(DISTDIR)/ReadMe.txt
