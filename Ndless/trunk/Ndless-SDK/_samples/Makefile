SUBDIRS = colors helloworld helloworld-sdl helloworld-cpp hellworld-asm link-sdl luaext particles 

all:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
  (cd $$i; make all) || exit 1; done

clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; make clean) || exit 1; done
