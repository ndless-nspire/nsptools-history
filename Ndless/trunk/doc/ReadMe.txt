=============================
= Ndless for TI-Nspire v1.7 =
=============================

What is it?
===========

Ndless combines an executable loader and utilities to open the TI-Nspire to 
third-party C and assembly development.

What do I need to install it on a TI-Nspire?
============================================

- A TI-Nspire CAS or non-CAS
- OS v1.7 installed on it
- Any linking program such as TI-Nspire Computer Link Software

How do I install it?
====================

Make sure OS v1.7 is installed.

Send with the linking program the documents 'ndless_installer.tns' and 
'ndless_resources.tns' from the calcbin/ directory of Ndless to any directory of 
the calculator. The two documents must be in the same directory.

Open 'ndless_installer' from the Documents screen of the calculator and wait a 
few seconds. Now:

 - the calculator may display a frozen clock and possibly random pixels at the 
 top of the screen. You are out of luck, the installation failed. Reboot the TI-
 Nspire by unplugging the USB cable, then removing and putting back a battery. 
 Retry the installation again.
 
 - The calculator may hot-reboot. Ndless is now installed.

The installation is not permanent: you need to reinstall Ndless after each 
reboot.

How do I upgrade from v1.0/v1.1?
================================

Install OS v1.7, then follow the installation steps above.

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable with a linking program to any folder of the TI-Nspire 
- Run the executable from the Documents screen

You may try the particles demo available in the calcbin/samples/ directory.

How do I uninstall it?
======================

Make sure the documents 'ndless_installer.tns' and 'ndless_resources.tns' are 
still in the 'ndless' directory on the calculator.
Then open 'ndless_installer' from the Documents screen. A full reboot will be 
initiated.

Troubleshooting
===============

- If the TI-Nspire becomes unusable, hangs or loops at reboot time:
  * Reboot the TI-Nspire, by unplugging the USB cable, then removing and putting
    back a battery.
  * Enter the maintenance menu by turning it on while holding the following keys 
    until half the progress bar:
      with the ClickPad keypad: Home+Enter+P then ON
      with the TouchPad keypad: Doc+Enter+EE then ON
  * Select '2' to uninstall the current OS. Reinstall it when asked to.
- If running an executable from the documents screen displays the following 
  message: "Sorry. Could not open document 'xxx.tns'", Ndless has not been 
  correctly installed. Run 'ndless_installer' again after a reboot.

How do I set up a development environment?
==========================================

Here is the preferred way for Windows:
- Add the sdk/bin/ directory to your PATH environment variable
- Install MSYS, the lightweight Unix-like shell environment: 
  http://www.mingw.org/wiki/msys
  The automated installation has been discontinued since MSYS 1.0.11, so you may 
  prefer to use this old version for an easier installation.
  Then you may optionally download the indivual component upgrades from 
  http://sourceforge.net/projects/mingw/files/ (MSYS sub-folder) and unpack them 
  in MSYS's installation directory using 7-zip
  (http://www.7-zip.org/download.html).
  MinGW is not required by Ndless.
- Install the YAGARTO GNU ARM toolchain - http://www.yagarto.de . Request 
  YAGARTO's installer to add YAGARTO's bin/ directory to your PATH environment 
  variable.
- You can now open "MSYS (rxvt)" from the Windows Start menu, and run "make" 
  from the root of a Makefile-based project.

To set up the development environment on Linux:
- Install the GNU ARM toolchain (this procedure may help:
  http://blog.nutaksas.com/2009/05/installing-gnuarm-arm-toolchain-on.html)
- Add the sdk/bin/ directory to your PATH environment variable
  (http://www.troubleshooters.com/linux/prepostpath.htm)

Ndless provides a light wrapper to some YAGARTO tools and TI-Nspire-specific C 
and assembly header files.

What you need to know as a developer
====================================

- An example of build script can be found in src/samples/hello/Makefile
- BSS sections are not allocated dynamically: global variables will make your
  programs bigger.
- Global variables with initialization data which requires relocation (for 
  example an array of function pointers must be relocated manually with 
  nl_relocdata(). See include/os.h.
- Pure-assembly programs must define the global symbol "main" after the header:
  see src/samples/hella/hella.S
- You may find interesting macros, inline functions and syscalls definition in 
  the include/ directory.
- Make sure that the assembly files extensions are in uppercase (.S) to make 
  them be preprocessed by the C preprocessor on which Ndless include files are 
  built
- OS functions ("syscalls") can be called, see the source code of the sample 
  programs in src/samples. Calling syscalls in thumb state is supported.
- If you want to use syscalls currently not implemented by Ndless, use the macro 
  SYSCALL_CUSTOM (see include/os.h)
- Compatibility with newlib has not been tested. You may get definition 
  conflicts and crashes due to newlib running without relocation. You should 
  build your programs with the nspire-ld flag "-nostdlib", except if
  you need the single precision floating-point helper functions (__aeabi_fadd, ...)
- Development information and resources are or will be available on Hackspire: 
    http://hackspire.unsads.com

Upgrading your developments and scripts
=======================================

The executable format, the conventions and the header files are currently being 
defined and prone to change. This section describes the upgrade steps between 
the different releases of Ndless.

* From v1.1 to v1.7:

C and assembly programs:
 - The syscall convention has changed, programs built with Ndless < v1.7 need to 
   be rebuilt without any change to the code. This format should be compatible 
   with the new OS versions once supported by Ndless, as long as backward 
   compatibility can be kept.
 - Custom syscalls should be defined using SYSCALL_CUSTOM (see above)
 - The "PRG\0" signature before main() isn't required anymore, you can remove ir
 - MakeTNS doesn't exist anymore. You must objcopy directly to the .tns file in 
   your Makefile.
 - The program format is not specific to an hardware model anymore. You can now 
   build your programs only once without defining NSPIRE_HARDWARE.
 - OS v1.1 is not supported anymore. Check that your programs still work on OS 
   v1.7.
Assembly programs:
 - The way to call syscalls has chanded: replace 'oscall <os_function>' with 
   'syscall(<os_function>)'

* From v1.0 to v1.1:

C and assembly programs:
 - Install the toolchain as described above. Your project doesn't need to follow 
   the Ndless file tree anymore.
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
- No reinstallation required after reboot
- More syscalls definitions
- Basic routines as static libraries
- A fully-fledged executable runner, with complete relocation support, library 
  support, an exception handler...
- Support for the TI-84+ keypad

We are open to any contribution to these features.

Many thanks to
==============

calc84maniac (GB/GBC emulator), Goplat (TI-Nspire emulator, HW info), hwti (OS 
info, 64-bit support), squalyl (Hackspire and development tools hosting),
TI-Bank (data hosting), all the contributors to the include files and all the 
beta testers.

Development team
================

Olivier Armand aka ExtendeD  -  < olivier dot calc at gmail dot com >
Geoffrey Anneheim aka geogeo - < geoffrey dot anneheim at free dot fr >

We will gladly accept PayPal donations for the hardware expenses that were 
required for the development of Ndless. Thank you for your support.

Legal stuff
===========

Most of the work is covered by the Mozilla Public License, version 1.1 (MPL). 
Please read careful the file "Mozilla-Public-License-v1.1.html" before 
distributing of any part of Ndless, with or without modification.

Some parts are covered by other licenses. Others are in the public domain. These 
parts are identified by the files LICENSE.txt or LICENSE.html in the sub-
directory.

Changelog
=========

* v1.7 - <unreleased>
  - NEW: Supports OS v1.7
  - NEW: A computer (or Nspire8x) isn't required anymore, the installation 
         is much more easier and compatible with nspire_emu.
  - NEW: Automated tests
 Hook:
  - NEW: the program format is not specific to an OS or hardware version anymore
  - NEW: syscalls can be called in thumb state
  - NEW: Ndless can exposed internal functions to programs (nl_...())
 Tools:
  - NEW: startup code relocates the program. You can now use C global variables.
         initialization data depending on relocation must be relocated with 
         nl_relocdata().
  - NEW: option --no-startup for nspire-ld
  - CHG: option -fpic moved to nspire-gcc
  - CHG: MakeTNS removed
 Samples:
  - NEW: sample program 'hello'
  - CHG: 'demo' renamed to 'particles' and moved to samples/
 Includes:
  - NEW: stat, NU_Get_First, NU_Get_Next, NU_Done, strcpy, strcmp, strlen, 
         strncat, exit
 - NEW: va_list, va_start, va_arg, va_end (thanks apcalc)
  - FIX: isKeyPressed could sometimes hang in while loops (missing 
         volatile keyword)

* v1.1 - 2010/07/31
 Installer:
  - NEW: Compatible with Computer Link Software 1.4
  - NEW: Possible manual step to work around missed reboots
  - FIX: Installer doesn't force downgrade anymore when already on OS v1.1
  - FIX: Some transfer errors were not correctly reported
 Loader:
  - FIX: The OS could not be upgraded while Ndless was installed
 Tools:
  - NEW: Building executables made a little easier. See the demo's Makefile and 
         "How do I set up a development environment?" section above.
  - NEW: Supports YAGARTO v4.5.0
  - FIX: MakeTNS: better error checking
 Includes:
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
