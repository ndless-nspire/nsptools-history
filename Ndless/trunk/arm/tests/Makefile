GCC = nspire-gcc
GXX = nspire-g++
LD-BFLT = nspire-ld-bflt
LD = nspire-ld
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
GCCFLAGS = -Os -Wall -W -marm
LDFLAGS = 
EXE = ndless_tests.test.tns ndless_keys.tns ndless_tpad.tns ndless_tests.luax.tns ndless_luatests.tns ndless_popup_tests.tns ndless_docbrowser_tests.tns ndless_cpp_tests.tns
DISTDIR = ../../calcbin/tests
vpath %.tns $(DISTDIR)

all: distdir $(EXE) static

distdir:
	@mkdir -p $(DISTDIR)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $<

%.tns: %.lua
	luna $< $(DISTDIR)/$@

%.tns: %.o
	$(LD-BFLT) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

#ndless_cpp_tests.tns: ndless_cpp_tests.o
#	$(LD) --cpp $^ -o $(@:.tns=.elf) $(LDFLAGS)
#	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

ndless_cpp_tests.tns: ndless_cpp_tests.o
	$(LD-BFLT) --cpp $(LDFLAGS) $^ -o  $(DISTDIR)/$@

ndless_tests.test.tns: ndless_tests.o assert.o
	$(LD-BFLT) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

ndless_tests.luax.tns: ndless_tests.luax.o assert.o
	$(LD-BFLT) $(LDFLAGS) $^ -o  $(DISTDIR)/$@

static:
	@cp ReadMe.txt $(DISTDIR)
	@cp ndless.cfg.tns $(DISTDIR)

clean:
	rm -f *.o *.elf
	-(cd $(DISTDIR) && rm -f $(EXE))
	rm -f $(DISTDIR)/ReadMe.txt $(DISTDIR)/ndless.cfg.tns
