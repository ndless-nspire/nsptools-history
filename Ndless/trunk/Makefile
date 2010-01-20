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
  
clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean); done
