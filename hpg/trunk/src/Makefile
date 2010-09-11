export CC=nspire-gcc
export AS=nspire-as
export LD=nspire-ld
export AR=arm-none-eabi-ar

# Common flags
export ARM_ELF_CFLAGS=-Wall	-Os -mthumb-interwork -nostdlib
export ARM_ELF_ASFLAGS= -EL -k -mno-fpu -mthumb-interwork

# Main target of this makefile
TARGET=hpg
LIBTARGET= lib$(TARGET).a
# Modules (ie subdirectories) used for the target
MODULES = common

all: thumb

install: thumb
	cp $(LIBTARGET) $(LIBS_PATH)
	cp *.h $(INCLUDE_PATH)

thumb: ARM_ELF_CFLAGS += -mthumb 
thumb: $(LIBTARGET)


arm_install: arm
	cp $(LIBTARGET) $(LIBS_PATH)
	cp *.h $(INCLUDE_PATH)

arm: ARM_ELF_CFLAGS += -marm
arm: $(LIBTARGET)


clean:
	@for I in ${MODULES}; do cd $$I && $(MAKE) $@ && cd ..; done;
	-@rm -rf $(LIBTARGET)
	-@rm -rf doc/


$(LIBTARGET) : */*.c 
	@for I in ${MODULES}; do cd $$I && $(MAKE) && cd ..; done;
	$(AR) rcv $(LIBTARGET) */*.o

doc:
	doxygen
