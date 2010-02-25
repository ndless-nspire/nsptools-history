===============================
= Ndless for TI-Nspire beta ? =
===============================

What is it?
===========

Ndless allows to run executables on the TI-Nspire hardware.
Ndless comes with an installer to prepare the TI-Nspire and install the executable
loader from a computer.

What do I need to install it?
=============================

- A TI-Nspire CAS or non-CAS
- Windows
- TI-Nspire Computer Link Software v1.3 or above (previous versions not tested)
- An Internet connection for automatic OS download if OS 1.1 is not currently installed on
  the TI-Nspire

How do I install/upgrade it?
============================

Users of beta version 1 or 2 should uninstall Ndless before the upgrade (see further).
Other versions can be upgraded directly. 

- If you don't have an Internet connection and OS 1.1 is not currently installed on the
  TI-Nspire, get it and copy it to 'userfiles\tinspire_1.1.tno' (the .tno extension is
  supported by both CAS and non-CAS models)
- Use the TI-Nspire keypad, and connect the TI-Nspire to the computer
- Run install-ndless.bat
- Follow the steps on the screen. Installing Ndless should take less than 2 minutes with
  few manual steps.

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable with a linking program to any folder of the TI-Nspire
- Run the executable from the Documents screen

A demo executable is automatically transferred by the installer to the documents folder
'ndless' you may want to try.

How do I uninstall it?
======================

To uninstall Ndless, reboot the OS while holding the Theta key (next to the ON key) when
the clock is displayed. The calculator will perform a full reboot.

What you need to know as a user
===============================

- The Ndless Installer creates tempory files in the documents folder 'ndless'.
  You may remove this folder after installation.
- If the TI-Nspire becomes unusable:
   * Enter the maintenace menu by reboot it while holding the keys ON+Home+Enter+P until
     half the progress bar. To reboot the TI-Nspire, unplug the USB cable, then remove and
     put back the a battery.
   * Select '2' to uninstall the current OS. Use the Ndless Installer or TI-Nspire Computer
     Link Software to reinstall an OS.
- If the Ndless Installer is lost in is installation sequence, restart it. Ndless may get
  installed before the end of the second sequence.
- If running an executable from the documents screen displays the following message:
  "Sorry. Could not open document 'xxx.tns'", Ndless has not been correctly installed.
  Run the installer again.

What you need to know as a developer
====================================

The executable format and conventions are currently being defined and prone to change.

- The YAGARTO GNU ARM toolchain is a good choice for C and assembly:
  http://www.yagarto.de
  An example of build script can be found in src\arm\demo\Makefile
- Executables must be position independent: don't use absolute addresses
- Executable files must start with the 4-bytes-long header 'PRG\0'. The utility 'MakeTNS'
  available in the tools directory may be used to add this header to a binary file.
- OS functions can be called from the program, see the source code of 'demo'.
- Development information and resources are or will be available on Hackspire: 
    http://hackspire.unsads.com

Installing Ndless on nspire_emu
===============================

Ndless can be installed on the third-party TI-Nspire emulator 'nspire_emu' with a
specific installation process.
This feature is currently in development and may be unstable.

- Create a new OS 1.1 flash image, run it once with nspire_emu. Choose 'English'
  as system language and save the image.
- Edit the file tools\FlashEdit\install-ndless-nspire_emu.bat and set the variables
  NSPIRE_EMU_FLASH_IMAGE_PATH and HARDWARE.
- Run install-ndless-nspire_emu.bat and follow the steps.

You should then be able to send executables and run them as on a real calculator.

If you are getting a "System Error" at boot time on nspire_emu, you are out of luck
and should wait for future versions of Ndless with better support (or even contribute
as a developer).


What we hope you will be able to see in the next releases
=========================================================

- Ndless installation on recent OS versions
- A comprehensive development toolchain
- A fully-fledged executable runner, with relocation and library support, an exception
  handler, version-independent OS calls, ...
- More stable and user-friendly installation on nspire_emu
- Support for the TI-84+ keypad
- Mac support

Known issues
============

- Sometimes the Ndless Installer cannot remote reboot the TI-Nspire, but thinks it has.
  In such case, navigate in any menu and the calculator should soon reboot and let you
  finish the installation process.

Many thanks to
==============

calc84maniac (GB/GBC emulator), Goplat (TI-Nspire emulator, HW info), hwti (OS info, 64-bit
support), squalyl (Hackspire and development tools hosting), TI-Bank (data hosting)

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

History
=======

* <unreleased>
 - The work is now covered by Open Source licenses

* beta 4 - <unreleased> 
 - The language can now be changed after the installation
 - Particle Demo written in C
 - Installation steps for nspire_emu

* beta 3 - <unreleased>
 - OS hook to run executables from the Documents screen
 - Fast upgrade by the installer if Ndless is already installed

* beta 2 - <unreleased> 
 - 64-bit architectures support thanks to hwti
