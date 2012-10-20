GCC = nspire-gcc
LD = nspire-ld-bflt
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS = 
EXE = ndless_tests.test.tns ndless_keys.tns ndless_tpad.tns ndless_tests.luax.tns ndless_luatests.tns ndless_popup_tests.tns ndless_docbrowser_tests.tns
DISTDIR = ../../calcbin/tests
vpath %.tns $(DISTDIR)

all: distdir $(EXE) static

distdir:
	@mkdir -p $(DISTDIR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.tns: %.lua
	luna $< $(DISTDIR)/$@

%.tns: %.o
	$(LD) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

ndless_tests.test.tns: ndless_tests.o assert.o ../config.o
	$(LD) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

ndless_tests.luax.tns: ndless_tests.luax.o assert.o
	$(LD) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

static:
	@cp ReadMe.txt $(DISTDIR)
	@cp ndless.cfg.tns $(DISTDIR)

clean:
	rm -f *.o *.elf
	-(cd $(DISTDIR) && rm -f $(EXE))
	rm -f $(DISTDIR)/ReadMe.txt $(DISTDIR)/ndless.cfg.tns
