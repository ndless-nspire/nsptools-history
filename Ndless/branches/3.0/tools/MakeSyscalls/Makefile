include ../../Makefile.config

# OS-specific
#OS_VERSIONS=ncas-1.7 cas-1.7 ncas-2.0.1 cas-2.0.1 ncas-2.1.0 cas-2.1.0 ncas-3.0.1 cas-3.0.1
OS_VERSIONS=cas-3.0.1

all: idb2idc syscalls syscalls_light

idb2idc:
	(cd $(IDC_FILES_DIR) && ./idb2idc.sh)

syscalls:
	./mksyscalls.sh $(IDC_FILES_DIR) ../../include/syscalls.h $(OS_VERSIONS)

syscalls_light:
	./mksyscalls.sh $(IDC_FILES_DIR) ../../include/syscalls-light.h $(OS_VERSIONS)

clean:
