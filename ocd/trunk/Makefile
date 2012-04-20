GCC = nspire-gcc
AR := "$(shell (which arm-elf-ar arm-none-eabi-ar arm-linux-gnueabi-ar | head -1) 2>/dev/null)"
LD = nspire-ld
GCCFLAGS = -Os -nostdlib -Wall -W -marm
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
LIB = libocd.a

all: $(LIB) install

dist: src all
	cp $(LIB) ocd.h ReadMe.txt dist

src: clean
	rm -rf dist
	mkdir -p dist/src
	cp *.c *.h Makefile dist/src

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $^

install:
	cp $(LIB) "${USERPROFILE}/.ndless/lib"
	cp ocd.h  "${USERPROFILE}/.ndless/include"

clean:
	rm -rf dist
	rm -f *.o *.elf *.a
	rm -f $(DISTDIR)/$(LIB)

upload:
	file=`ls ocd*.zip` && \
	curl --cookie upload_cookies.txt -F 'super_id=2' -F 'form_type=file' -F '__FORM_TOKEN=c30f9c971c7fca42db234011' -F "name=$$file" -F 'submit=Submit' -F "file_to_upload=@$$file" -F 'sort=' -F 'architecture=' -F 'notes=' http://www.unsads.com/projects/nsptools/admin/general/downloader/files/release > /dev/null;
	echo "Check http://www.unsads.com/projects/nsptools/downloader/download/release/2"
