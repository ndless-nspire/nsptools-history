=============================
= Ndless v3.1 for TI-Nspire =
=============================

What is it?
===========

Ndless combines an executable loader and utilities to open the TI-Nspire to 
third-party C and assembly development.

As a TI-Nspire user, installing Ndless on your calculator will enhance the 
Operating System with the functions required to run assembly programs until the 
next reboot.

What do I need to install it on a TI-Nspire?
============================================

- A TI-Nspire CAS or non-CAS Clickpad, Touchpad or CX
- OS v3.1.0 installed on it
- Any linking program such as TI-Nspire Student Software

How do I install it?
====================

Create a directory called 'ndless' on the calculator. Send the file 
'ndless_resources.tns' from the calcbin/ directory of Ndless to this new 
directory.

Then locate the file 'ndless_installer-<version>.tno|tnc|tco|tcc' in the 
calcbin/ directory of Ndless where <version> matches the OS version currently 
installed on the calculator.
 
Send this file as an OS upgrade to the calculator. The calculator wuill display 
a popup about the success of the installation. The linking software will display 
an error message. You may have to close and reopen it to use it again.

That's it!  The installation is not permanent: you need to reinstall Ndless 
after each reboot.

How do I install it on nspire_emu/Ncubate?
==========================================

Create a folder named 'ndless'. Set the target to 'ndless' (menu Link->Set 
target folder...). Send  'ndless_resources.tns' as a document, and 
'ndless_installer-.tno|tnc|tco|tcc' as an OS.

How do I upgrade from v2.0?
================================

Upgrade your OS version and install Ndless.

How do I run an executable?
===========================

Executables have the extension .tns as standard documents.

- Transfer the executable with a linking program to any folder of the TI-Nspire 
- Run the executable from the Documents screen

You may try the particles demo available in the calcbin/samples/ directory.

Programs built for Ndless v2.x should work as is.

How do I uninstall it?
======================

Then open 'ndless_resources.tns' from the Documents screen. A full reboot will be 
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

Known issues
============

- *No* popup must be displayed when ndless_installer-*.txx is sent (especially the 
  confirmation of the reception of 'ndless_resources.tns'), else Ndless's popup 
  will block (you can still dismiss it by turning off and on the calculator).
- Ndless v1.x/v2.x programs which use the show_msgbox() function must be rebuilt 
  to be compatible. Opening corrupted files will trigger a reboot instead of 
  displaying an error message.
- No OS can be sent as long as Ndless is installed

A more complete list of known isues can be found in the tracker:
http://www.unsads.com/projects/nsptools

Support and suggestions
=======================

Forum: http://www.omnimaga.org/index.php?board=136.0
Issues and enhancements tracker: http://www.unsads.com/projects/nsptools
General TI-Nspire hacking wiki: http://hackspire.unsads.com
Development version of the source code:
  https://www.unsads.com/scm/svn/nsptools/Ndless/trunk (guest/guest)

Installation problem report
===========================

First make sure:
- You are using the right 'ndless_installer-<version>.tno|tnc|tco|tcc' file 
  compatible with the Operating System (OS) version currently installed on your 
  calculator
- You have followed the steps in the troubleshooting section above
- You don't find a solution on the web sites above

Then count if you the number of dotted lines which appear at the bottom of the 
screen.

E-mail this with anything which might help the authors to fix the issue.

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

- More syscalls definitions
- Basic routines as static libraries
- A fully-fledged executable runner, with complete relocation support, library 
  support, an exception handler...
- Support for the TI-84+ keypad

We are open to any contribution to these features.

Many thanks to
==============

calc84maniac (GB/GBC emulator, HW info), Goplat (TI-Nspire emulator, HW info), 
hwti (OS info, v1.0 64-bit support), squalyl (Hackspire and development tools 
hosting), TI-Planet (data hosting).

Thanks to all the contributors and beta testers, including Adriweb, apcalc, 
BrandonW, bsl, bwang, calc84maniac, calcforth, chronomex, critor, Levak,
Lionel Debroux, shrear.

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
