include ../../Makefile.config

# OS-specific
OS_VERSIONS=ncas-1.7 cas-1.7

all: syscalls syscalls_light

syscalls:
	./mksyscalls.sh $(IDC_FILES_DIR) ../../include/syscalls.h $(OS_VERSIONS)

syscalls_light:
	./mksyscalls.sh $(IDC_FILES_DIR) ../../include/syscalls-light.h $(OS_VERSIONS)

clean:
	