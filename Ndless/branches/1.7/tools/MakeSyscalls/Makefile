include ../../Makefile.config

# OS-specific
OS_VERSIONS=ncas-1.7 cas-1.7

all: syscalls

syscalls:
	IDC_FILES_DIR=$(IDC_FILES_DIR) ./mksyscalls.sh $(OS_VERSIONS)

clean:
	