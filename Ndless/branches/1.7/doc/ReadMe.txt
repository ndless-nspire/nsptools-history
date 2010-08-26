=============================
= Ndless for TI-Nspire v1.1 =
=============================

What is it?
===========

Ndless combines a computer installer, an executable loader and utilities to open the
TI-Nspire to third-party C and assembly development.

What do I need to install it on a TI-Nspire?
============================================

- A TI-Nspire CAS or non-CAS
- OS v1.7 installed on it
- Any linking program such as TI-Nspire Computer Link Software

How do I install it?
====================

How do I upgrade from v1.0/v1.1?
================================

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable with a linking program to any folder of the TI-Nspire
- Run the executable from the Documents screen

You may try the particles demo available in TODO.

How do I uninstall it?
======================

To uninstall Ndless, reboot the OS while holding the Theta key (next to the ON key) when
the clock is displayed. The calculator will perform a full reboot.

Troubleshooting
===============

- If the TI-Nspire becomes unusable, hangs or loops at reboot time:
  * Reboot the TI-Nspire, by unplugging the USB cable, then removing and putting
    back a battery.
  * Enter the maintenance menu by turning it on while holding the following keys until
    half the progress bar:
      with the ClickPad keypad: Home+Enter+P then ON
      with the TouchPad keypad: Doc+Enter+EE then ON
  * Select '2' to uninstall the current OS. Reinstall it when asked to.
- If running an executable from the documents screen displays the following message:
  "Sorry. Could not open document 'xxx.tns'", Ndless has not been correctly installed.
  Run ndless_installer again after a reboot.

How do I set up a development environment?
==========================================

Here is the preferred way for Windows:
- Add the bin/ folder of Ndless to your PATH environment variable
- Install MSYS, the lightweight Unix-like shell environment: http://www.mingw.org/wiki/msys
  The automated installation has been discontinued since MSYS 1.0.11, so you may prefer to
  use this old version foro an easier installation.
  Then you may optionally download the indivual component upgrades from
  http://sourceforge.net/projects/mingw/files/ (MSYS sub-folder) and unpack them in MSYS's
  installation directory using 7-zip (http://www.7-zip.org/download.html).
  MinGW is not required by Ndless.
- Install the YAGARTO GNU ARM toolchain - http://www.yagarto.de . Request YAGARTO's
  installer to add YAGARTO's bin/ directory to your PATH environment variable.
- You can now open "MSYS (rxvt)" from the Windows Start menu, and run "make" from the root
  of a Makefile-based project.

To setup the developement environment on Linux:
- Install the GNU ARM toolchain (this procedure may help:
  http://blog.nutaksas.com/2009/05/installing-gnuarm-arm-toolchain-on.html )
- Add the bin/ folder of Ndless to your PATH environment variable
  (http://www.troubleshooters.com/linux/prepostpath.htm)

Ndless provides a light wrapper to some YAGARTO tools and TI-NSpire-specific C and assembly
header files.

What you need to know as a developer
====================================

- An example of build script can be found in src/samples/particles/Makefile
- Executable files must start with the 4-bytes-long header 'PRG\0', with their entry
  point right after it. The utility 'MakeTNS' available in the tools directory may be
  used to skip newlib's startup code added before this signature.
- Pure-assembly programs must define the global symbol "main" after the header:
	.string "PRG"
main: .global main
	<...main code...>
- You may find interesting macros, inline functions and syscalls definition in the
  include/ directory.
- Make sure that the assembly files extensions are in uppercase (.S) to make them
  be preprocessed by the C preprocessor on which Ndless include files are built
- OS functions can be called from the program, see the source code of 'demo'.
- Development information and resources are or will be available on Hackspire: 
    http://hackspire.unsads.com

Upgrading your developments and scripts
=======================================

The executable format, the conventions and the header files are currently being defined
and prone to change. This section describes the upgrade steps between the different
releases of Ndless.

* From v1.1 to v1.7:
C and assembly programs:
 - The syscall convention has changed, programs built with Ndless < v1.7 need to be
   rebuilt without any change to the code. This format should be compatible with the new
   OS versions once supported by Ndless, as long as backward compatibility can be kept.
 - The program format is not specific to an hardware model anymore. You can now build
   your programs only once without defining NSPIRE_HARDWARE.
 - OS v1.1 is not supported anymore. Check that your programs still work on OS v1.7.

* From v1.0 to v1.1:

C and assembly programs:
 - Install the toolchain as described above. Your project doesn't need to follow the
   Ndless file tree anymore.
 - Adapt your Makefile based on src/arm/Makefile or src/arm/demo/Makefile
 - You may upgrade to the latest version of YAGARTO. Don't forget to delete any
   remaining *.o file before rebuilding a project.
Pure-assembly programs:
 - Make sure that the file extensions is in uppercase (.S)
 - Add the "main" symbol as described in the previous section
 - You do not need to define the symbol "_start" anymore

What we hope you will be able to see in the next releases
=========================================================

- Installation on recent OS versions
- More syscalls definitions
- Basic routines as static libraries
- A fully-fledged executable runner, with library support, an exception handler...
- Support for the TI-84+ keypad

We are open to any contribution to these features.

Many thanks to
==============

calc84maniac (GB/GBC emulator), Goplat (TI-Nspire emulator, HW info), hwti (OS info, 64-bit
support), squalyl (Hackspire and development tools hosting), TI-Bank (data hosting), all
the contributors to the include files and all the beta testers.

Development team
================

Olivier Armand aka ExtendeD  -  < olivier dot calc at gmail dot com >
Geoffrey Anneheim aka geogeo - < geoffrey dot anneheim at free dot fr >

We will gladly accept PayPal donations for the hardware expenses that were required for
the development of Ndless. Thank you for your support.

Legal stuff
===========

Most of the work is covered by the Mozilla Public License, version 1.1 (MPL).
Please read careful the file "Mozilla-Public-License-v1.1.html" before the distribution of
any part of Ndless, with or without modification.

Some parts are covered by other licenses. Others are in the public domain.
These parts are identified by the files LICENSE.txt or LICENSE.html in the sub-directory.

Changelog
=========

* v1.7 - <unreleased>
  - NEW: Support of OS v1.7
  - NEW. A computer (or Nspire8x) isn't required anymore, and the installation is much
    more easier
 Hook:
  - NEW: the program format is not specific to an OS or hardware version anymore
 Tools:
  - NEW: startup code relocates the program. You can now use C global variables.
  - NEW: option --no-startup for nspire-ld
  - FIX: option -fpic was missing in nspire-gcc from v1.1
 Samples:
  - CHG: 'demo' renamed to 'particles' and moved to samples/
 Include files:
  - NEW: stat, NU_Get_First, NU_Get_Next, NU_Done

* v1.1 - 2010/07/31
 Installer:
  - NEW: Compatible with Computer Link Software 1.4
  - NEW: Possible manual step to work around missed reboots
  - FIX: Installer doesn't force downgrade anymore when already on OS v1.1
  - FIX: Some transfer errors were not correctly reported
 Loader:
  - FIX: The OS could not be upgraded while Ndless was installed
 Tools:
  - NEW: Building executables made a little easier. See the demo's Makefile and "How do I 
     set up a development environment?" section above.
  - NEW: Supports YAGARTO v4.5.0
  - FIX: MakeTNS: better error checking
 Include files:
  - NEW: Added fgets, stdin, stdout, stderr (thanks bsl)
  - NEW: Added printf, fprintf (thanks bwang)
  - NEW: Exhaustive key constants (thanks bwang)
  - NEW: added halt, idle
  - NEW: (ARM) added halt
  - FIX: log_rs232, fread

* v1.0 - 2010/02/26
 - The work is now covered by Open Source licenses
 - Fixed a potential endless OS reboot after a system language change

* beta 4 - <unreleased> 
 - The language can now be changed after the installation
 - Particle Demo written in C
 - Installation steps for nspire_emu

* beta 3 - <unreleased>
 - OS hook to run executables from the Documents screen
 - Fast upgrade by the installer if Ndless is already installed

* beta 2 - <unreleased> 
 - 64-bit architectures support thanks to hwti
