ifndef HPGCC_PATH
export HPGCC=../../..
else
export HPGCC=$(HPGCC_PATH)
endif

INCLUDE_PATH=$(HPGCC)/include
LIBS_PATH=$(HPGCC)/lib
ELF2HP_PATH=$(HPGCC)/elf2hp

LIBS= -lhpg -lhplib -lgcc
ELF2HP= ${ELF2HP_PATH}/elf2hp.exe

ifeq ($(SHELL),)
  $(warning Compiler detection does not work on Windows)
  $(warning Assuming CC_TARGET=arm-elf)
  CC_TARGET=arm-elf
endif

ifeq ($(CC_TARGET),)
  ifneq ($(shell which arm-linux-gcc),)
    CC_TARGET=arm-linux
  else
    ifneq ($(shell which arm-elf-gcc),)
      CC_TARGET=arm-elf
    else
      $(error No compiler found. Check PATH or provide a CC_TARGET)
    endif
  endif
endif

export CC=$(CC_TARGET)-gcc
export AS=$(CC_TARGET)-as
export LD=$(CC_TARGET)-ld
export AR=$(CC_TARGET)-ar


ifeq ($(PLATFORM),cygwin)
endif

export CUR_DIR= $(shell pwd)
export ARM_ELF_CFLAGS= -mtune=arm920t -mcpu=arm920t \
	-mapcs-32 -msingle-pic-base -fpic -mpic-register=r10 \
	-mlittle-endian -fomit-frame-pointer -msoft-float -Wall \
	-O3 
export ARM_ELF_ASFLAGS= -EL -k -mcpu=arm920t -mno-fpu -mapcs-32
export ARM_ELF_LDFLAGS= -nodefaultlibs -nostdlib


SRC = $(shell echo *.c)

OBJ=$(SRC:%.c=%.o)

EXE=$(SRC:%.c=%.exe)

HP=$(SRC:%.c=%.hp)


all: $(HP)

install: all

clean:
	rm -rf *.o *.a *.exe *.hp

%.o : %.c
	$(CC) ${ARM_ELF_CFLAGS} -I$(INCLUDE_PATH) -I.. -c $< -o $@

%.exe : %.o
	$(LD) -L$(LIBS_PATH) -TMMUld.script $(LIBS_PATH)/crt0.o ${ARM_ELF_LDFLAGS} $< $(LIBS) -o $@

%.hp : %.exe
	$(ELF2HP) -k $< $@


depend:
	$(CC) -MM -I$(INCLUDE_PATH) -I.. $(SRC) > Dependencies

include Dependencies
