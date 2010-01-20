===============================
= Ndless for TI-Nspire beta 2 =
===============================

What is it?
===========

Ndless allows to run arbitrary code on the TI-Nspire hardware.
Ndless comes with an installer to prepare the TI-Nspire and download the executable
automatically from a computer.

What do I need to run it?
=========================

- A TI-Nspire CAS or non-CAS
- Windows
- TI-Nspire Computer Link Software v1.3 or above (previous versions not tested)
- An Internet connection for automatic OS download if OS 1.1 is not currently installed on
  the TI-Nspire

How to use?
===========

- Copy the raw binary file contains ARM instructions to 'userfiles\runme.tns'. If you don't
  have one, a sample program will be downloaded to the TI-Nspire (the source
  code is available in 'arm\demo.s')
- If you don't have an Internet connection and OS 1.1 is not currently installed on the
  TI-Nspire, get it and copy it to 'userfiles\tinspire_1.1.9253.tno'.
- Use the TI-Nspire keyboard, and connect the TI-Nspire it to the computer
- Run install-ndless.bat
- Follow the steps on the screen. Installing Ndless should take less than 2 minutes with
  few manual steps.
- At the last reboot, the sample program or your custom 'runme.tns' will be executed during OS
  startup sequence.
- To uninstall the Ndless hook, reboot the OS while holding the Theta key (next to the ON key).
  To load a new executable, you will need to reinstall Ndless with its installer.

What you need to know as a user
===============================

- The Ndless Installer and Ndless create files in the documents directory
  'ndless-installation'
- If the TI-Nspire becomes unusable, reboot it while holding the keys ON+Home+Enter+P.
  Then select '2' to uninstall the current OS. Use the Ndless Installer or TI-Nspire Computer
  Link Software to reinstall an OS.
- If the Ndless Installer is lost in is installation sequence, restart it.

What you need to know as a developer
====================================

- OS functions can be called from the program run (see the sample include file 'arm\os.s')


What you can't currently do, but should soon I be able to
=========================================================

- Upgrade to a more recent version of the OS and while keeping Ndless installed
- Install or run the executable with the TI-84+ keyboard of the TI-Nspire

What is not yet planned
=======================

- Mac support

Known issues
============

- Sometimes the Ndless Installer cannot remote reboot the TI-Nspire. If it doesn't after
  a few seconds, navigate in any menu and it should soon reboot.

Many thanks to
==============

calc84maniac (GB/GBC emulator), Goplat (TI-Nspire emulator, HW info), hwti (OS info, 64-bit
support), squalyl (Hackspire and development tools hosting), TI-Bank (data hosting)

Ndless is collaborative achievement.

Development team
================

Olivier Armand aka ExtendeD - < olivier dt calc hat gmail dt com >
Geoffrey Anneheim aka geogeo - < geoffrey dt anneheim hat free dt fr>

History
=======

* - beta 2
 64-bit architectures support thanks to hwti

* 2010-01-12 - beta 1
 First release
