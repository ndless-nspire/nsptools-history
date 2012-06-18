SUBDIRS = tools libndls arm samples
SUBDIR_TOOLS = tools
SUBDIRSCLEAN = $(SUBDIRS)
DISTDIRS = calcbin
SDKDISTDIRS = bin lib
SDKDIRS = $(SDKDISTDIRS) include system

all: subdirs

.PHONY: subdirs

subdirs:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

all_tools:
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
	find dist -name Makefile.config -o -name upload_cookies.txt  | xargs rm -rf

cleandist:
	rm -rf dist

clean: cleandist
	@for i in $(SUBDIRSCLEAN); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
	@# may fail because of nspire_emu keeping a lock on it
	-rm -rf calcbin
	rm -rf $(SDKDISTDIRS)

# Useful shortcuts
.PHONY: libndls
libndls:
	(cd libndls && make)
rlibndls:
	(cd libndls && make clean all)

.PHONY: samples
samples:
	(cd samples && make)
rsamples:
	(cd samples && make clean all)

.PHONY: tests
tests:
	(cd arm/tests && make)
rtests:
	(cd arm/tests && make clean all)

upload: update_version_info dist
	svn update
	svnrev=`svn info --xml |grep revision | uniq | sed 's/\(revision="\)//' | sed 's/">//' | sed 's/[[:blank:]]*//g'`; \
	mv dist "ndless-v3.1-beta-r$$svnrev"; \
 	rm -rf ndless.zip ; \
	7z a ndless.zip "ndless-v3.1-beta-r$$svnrev"; \
	curl --cookie upload_cookies.txt -F 'super_id=1' -F 'form_type=file' -F '__FORM_TOKEN=c30f9c971c7fca42db234011' -F "name=ndless-v3.1-beta-r$$svnrev.zip" -F 'submit=Submit' -F 'file_to_upload=@ndless.zip' -F 'sort=' -F 'architecture=' -F 'notes=' http://www.unsads.com/projects/nsptools/admin/general/downloader/files/release > /dev/null; \
	#rm -rf ndless.zip; \
	rm -rf "ndless-v3.1-beta-r$$svnrev"
	echo "Check http://www.unsads.com/projects/nsptools/downloader/download/release/1"

update_version_info:
	@svn update
	@svnrev=`svn info --xml |grep revision | uniq | sed 's/\(revision="\)//' | sed 's/">//' | sed 's/[[:blank:]]*//g'`; \
	echo "#define NDLESS_REVISION $$((svnrev+1))" > arm/ndless_version.h
	svn commit -m "ndless_version.h: update revision" arm/ndless_version.h
