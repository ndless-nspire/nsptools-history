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

Ndless is currently not compatible with Touchpad TI-Nspire models.

How do I install it?
====================

Make sure OS v1.7 is installed.

Send with the linking program the documents 'ndless_installer.tns' and 
'ndless_resources.tns' from the calcbin/ directory of Ndless to any directory of 
the calculator. The two documents must be in the same directory.

Unplug the calculator from the computer (it may be required for some 
configurations).

Open 'ndless_installer' from the Documents screen of the calculator and wait a 
few seconds. Now:

 - The calculator may display a popup about the success of the installation.
 That's it!
 The installation is not permanent: you need to reinstall Ndless after each 
 reboot.

 - The calculator may display a frozen clock and possibly 1, 2 or 3 vertical 
 bars of random pixels on the screen. You are out of luck, the installation 
 failed. The TI-Nspire should reboot by itself after a few seconds.
 If not, force a reboot by unplugging the USB cable if plugged in, then removing 
 and putting back a battery. Retry the installation again.
 If you can't install Ndless after multiple attempts, jump to the section 
 'Problem report' below.

How do I upgrade from v1.0/v1.1?
================================

Install OS v1.7, then follow the installation steps above.

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable with a linking program to any folder of the TI-Nspire 
- Run the executable from the Documents screen

You may try the particles demo available in the calcbin/samples/ directory.

Programs built for Ndless v1.0/v1.1 won't work with this version until they are 
updated by their authors.

How do I uninstall it?
======================

Make sure the documents 'ndless_installer.tns' and 'ndless_resources.tns' are 
still in the same directory on the calculator.
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

Developing for the TI-Nspire
============================

Development information and resources are available on Hackspire: 
    http://hackspire.unsads.com
Join the community now!

Upgrading your developments and scripts
=======================================

The executable format, the conventions and the header files are currently being 
defined and prone to change. The upgrade steps between the different releases 
are documented here: http://hackspire.unsads.com/wiki/index.php/Ndless

What we hope you will be able to see in the next releases
=========================================================

- Installation on recent OS versions
- More syscalls definitions
- Basic routines as static libraries
- A fully-fledged executable runner, with complete relocation support, library 
  support, an exception handler...
- Support for the TI-84+ keypad

We are open to any contribution to these features.

Many thanks to
==============

calc84maniac (GB/GBC emulator, HW info), Goplat (TI-Nspire emulator, HW info), 
hwti (OS info, 64-bit support), squalyl (Hackspire and development tools 
hosting), TI-Bank (data hosting).

Thanks to all the contributors and beta testers: apcalc, bsl, bwang, 
calc84maniac, Levak.

Development team
================

  Olivier Armand aka ExtendeD  -  < olivier dot calc at gmail dot com >

Former team member:
  Geoffrey Anneheim aka geogeo - < geoffrey dot anneheim at free dot fr >

Ndless is brought to you by Omnimaga: The Coders Of Tomorrow
                        www.omnimaga.org

We will gladly accept PayPal donations for the hardware expenses that were 
required for the development of Ndless. You can use the e-mail addresses above 
or the following URL:
https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=PJ7SGQPQJ8WKQ&item_name=Ndless%20Donation
Thank you for your support.

Problem report
==============

First make sure:
- You have followed the steps in the troubleshooting section above
- You don't find a solution either on http://hackspire.unsads.com
  or http://www.omnimaga.org/index.php?board=136.0

Then count if you can:
- the number of vertical bars of random pixels which appear at the top of the 
  screen at installation time
- The number of dotted lines which appear at the bottom of the screen

E-mail this with anything which might help the authors to fix the issue.

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

See Changes.txt.
