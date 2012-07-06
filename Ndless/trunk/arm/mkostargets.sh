#!/bin/sh
# Generate verbose Makefile targets for the different OS versions and models

get_ext() {
		case $1 in
		ncas)   echo tno;;
		cas)    echo tnc;;
		ncascx) echo tco;;
		cascx)  echo tcc;;
		esac
}

# OS-specific
OS_VERSIONS="3.1.0"
MODELS="ncas cas ncascx cascx"

echo "OS_VERSIONS := $OS_VERSIONS"
echo "MODELS := $MODELS"

extensions=".tno .tnc .tco .tcc"

for os_version in $OS_VERSIONS; do
	for model in $MODELS; do
		installer_files="$installer_files ndless_installer-$os_version.`get_ext $model`"
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
		extension=`get_ext $model`
		echo "ndless_installer-${os_version}.${extension}: OS_NAME=$os_version-$model"
		echo "ndless_installer-${os_version}.${extension}: OS_VERSION=$os_version"
		echo "ndless_installer-${os_version}.${extension}: MODEL=$model"
		echo "ndless_installer-${os_version}.${extension}: ndless_installer-${os_version}-$model.elf"
	done
done
