===============================
= Ndless for TI-Nspire beta 3 =
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

How do I install it?
====================

- If you don't have an Internet connection and OS 1.1 is not currently installed on the
  TI-Nspire, get it and copy it to 'userfiles\tinspire_1.1.tno' (the .tno extension is
  supported by both CAS and non-CAS models)
- Use the TI-Nspire keypad, and connect the TI-Nspire to the computer
- Run install-ndless.bat
- Follow the steps on the screen. Installing Ndless should take less than 2 minutes with
  few manual steps.
- To uninstall Ndless, reboot the OS while holding the Theta key (next to the ON key).

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable to any folder of the TI-Nspire
- Run the executable from the Documents screen

A demo executable can be found in Ndless package (res/<TI-Nspire model>/demo.tns).

What you need to know as a user
===============================

- The Ndless Installer creates files in the documents directory 'ndless'.
  This directory should not be removed as long as Ndless is installed.
- If the TI-Nspire becomes unusable:
   * Reboot it while holding the keys ON+Home+Enter+P. To reboot the TI-Nspire, unplug the
     USB cable, then remove and put back the a battery
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

- Executables must be position independent: don't use absolute addresses
- Executable files must start with the 4-bytes-long header 'PRG\0'
- OS functions can be called from the program run (see the sample include file 'arm\os.s')
- Development information and resources are or will be available on Hackspire: 
    http://hackspire.unsads.com

What you can't currently do, but should soon I be able to
=========================================================

- Upgrade to a more recent version of the OS and keep Ndless installed
- Use a fully-fledged executable runner, with relocation and library support, an exception
  handler, version-independent OS calls, ...
- Run executables with the TI-84+ keyboard of the TI-Nspire
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

History
=======

* <unreleased>
 - The language can now be changed after the installation

* beta 3 - <unreleased>
 - OS hook to run executables from the Documents screen
 - Fast upgrade by the installer if Ndless is already installed

* beta 2 - <unreleased> 
 - 64-bit architectures support thanks to hwti
