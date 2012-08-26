SUBDIRS = tools libndls arm Ndless-SDK/_samples
SUBDIR_TOOLS = tools
SUBDIRSCLEAN = $(SUBDIRS)
DISTDIRS = calcbin

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

distdir:
	mkdir -p dist	

distsamples: distdir samples
	mkdir -p dist/samples
	cp Ndless-SDK/_samples/particles/particles.tns dist/samples

# Incremental binary dist for development
distbin: distdir all distsamples
	@# system's artefacts shouldn't be distributed
	(cd Ndless-SDK/ndless/system && make clean)
	cp -r $(DISTDIRS) dist
	cp "Mozilla-Public-License-v1.1.html" doc/ReadMe.txt doc/Changes.txt doc/ndless-particle-demo.gif dist
	find dist -name .svn -o -name "*~" | xargs rm -rf

# Dist with cleanup, binary and source
dist: cleandist distsrc distbin
	find dist -name .svn | xargs rm -rf

distsrc: clean
	mkdir -p dist/src
	cp -r `ls | grep -v dist | grep -v Ndless-SDK` dist/src
	find dist -name Makefile.config -o -name upload_cookies.txt | xargs rm -rf

distsdk: dist
	mkdir -p distsdk
	cp -r Ndless-SDK/* distsdk
	find distsdk -name .svn -o -name "*~" -o -name "*.img.tns" -o -name "*.img" -o -name "*.tcc" | xargs rm -rf

cleandist:
	rm -rf dist

cleandistsdk:
	rm -rf distsdk

clean: cleandist cleandistsdk
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
	(cd Ndless-SDK/_samples && make)
rsamples:
	(cd Ndless-SDK/_samples && make clean all)

.PHONY: tests
tests:
	(cd arm/tests && make)
rtests:
	(cd arm/tests && make clean all)

upload: update_version_info dist distsdk
	svn update
	svnrev=`svn info --xml |grep revision | uniq | sed 's/\(revision="\)//' | sed 's/">//' | sed 's/[[:blank:]]*//g'`; \
	mv dist "ndless-v3.1-beta-r$$svnrev"; \
	mv distsdk "ndless-v3.1-beta-r$${svnrev}-sdk"; \
 	rm -rf ndless.zip ; \
	rm -rf ndless-sdk.zip ; \
	7z a ndless.zip "ndless-v3.1-beta-r$$svnrev"; \
	7z a ndless-sdk.zip "ndless-v3.1-beta-r$${svnrev}-sdk"; \
	curl --cookie upload_cookies.txt -F 'super_id=1' -F 'form_type=file' -F '__FORM_TOKEN=7df6886ff34bf2f3b93a343d' -F "name=ndless-v3.1-beta-r$$svnrev.zip" -F 'submit=Submit' -F 'file_to_upload=@ndless.zip' -F 'sort=' -F 'architecture=' -F 'notes=' http://www.unsads.com/projects/nsptools/admin/general/downloader/files/release > /dev/null; \
	#curl --cookie upload_cookies.txt -F 'super_id=1' -F 'form_type=file' -F '__FORM_TOKEN=f9cf1b8c51890f79c2a0008e' -F "name=ndless-v3.1-beta-r$${svnrev}-sdk.zip" -F 'submit=Submit' -F 'file_to_upload=@ndless-sdk.zip' -F 'sort=' -F 'architecture=' -F 'notes=' http://www.unsads.com/projects/nsptools/admin/general/downloader/files/release > /dev/null; \
	rm -rf ndless.zip; \
	#rm -rf ndless-sdk.zip; \
	rm -rf "ndless-v3.1-beta-r$$svnrev" \
	rm -rf "ndless-v3.1-beta-r$${svnrev}-sdk" \
	echo "Check http://www.unsads.com/projects/nsptools/downloader/download/category/1"

update_version_info:
	@svn update
	@svnrev=`svn info --xml |grep revision | uniq | sed 's/\(revision="\)//' | sed 's/">//' | sed 's/[[:blank:]]*//g'`; \
	echo "#define NDLESS_REVISION $$((svnrev+1))" > arm/ndless_version.h
	svn commit -m "ndless_version.h: update revision" arm/ndless_version.h
