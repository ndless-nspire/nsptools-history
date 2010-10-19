SUBDIRS = tools arm samples
SUBDIR_TOOLS = tools
SUBDIRSCLEAN = $(SUBDIRS)
DISTDIRS = calcbin
SDKDIRS = bin include system

all: subdirs
all_tools: subdirs_tools

.PHONY: subdirs

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
	@# system's artefacts shouldn't be distributed
	(cd system && make clean)
	cp -r $(DISTDIRS) dist
	mkdir -p dist/sdk
	cp -r $(SDKDIRS) dist/sdk
	cp "Mozilla-Public-License-v1.1.html" doc/ReadMe.txt doc/Changes.txt doc/ndless-particle-demo.gif dist
	find dist -name .svn -o -name "*~" | xargs rm -rf

# Dist with cleanup, binary and source
dist: cleandist distsrc distbin
	find dist -name .svn | xargs rm -rf

distsrc: clean
	mkdir -p dist/src
	cp -r `ls | grep -v dist` dist/src
	@# exclude some resources we don't want to distribute
	find dist -name Makefile.config -o -name ndless_tests.tns | xargs rm -rf

cleandist:
	rm -rf dist

clean: cleandist
	@for i in $(SUBDIRSCLEAN); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
	@# may fail because of nspire_emu keeping a lock on it
	-rm -rf calcbin
	rm -rf bin
