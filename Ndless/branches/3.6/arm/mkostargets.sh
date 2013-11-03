#!/bin/sh
# Generates verbose Makefile targets for the different OS versions and models

# OS-specific
OS_VERSIONS="3.6.0"
#MODELS="ncas cas ncascx cascx cmc cascmc"
MODELS="cascx"

echo "OS_VERSIONS := $OS_VERSIONS"
echo "MODELS := $MODELS"

for os_version in $OS_VERSIONS; do
	for model in $MODELS; do
		installer_files="$installer_files ndless_installer-$os_version-$model.tns"
	done
done
echo "INSTALLER_FILES := $installer_files"

# Since the installer is built specifically for an OS version, the version must be passed to the code.
# Unfortunately Makefile doesn't allow target-specific variable values to extract the stem from pattern rules,
for os_version in $OS_VERSIONS; do
	for model in $MODELS; do
		echo "ndless_installer-${os_version}-$model.elf: GCCFLAGS+=\$(call GCCFLAGS_INSTALLER,${os_version})"
	done
done

for os_version in $OS_VERSIONS; do
	for model in $MODELS; do
		echo "ndless_installer-${os_version}-$model.tns: OS_NAME=$os_version-$model"
		echo "ndless_installer-${os_version}-$model.tns: OS_VERSION=$os_version"
		echo "ndless_installer-${os_version}-$model.tns: MODEL=$model"
		echo "ndless_installer-${os_version}-$model.tns: ndless_installer-${os_version}-$model.lua"
		echo "ndless_installer-${os_version}-$model.lua: ndless_installer-${os_version}-$model.bin"
	done
done
