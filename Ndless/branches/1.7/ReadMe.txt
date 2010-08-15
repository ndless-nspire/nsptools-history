===============
= Ncubate Emu =
===============

What is it?
===========

Ncubate Emu is an enhanced of the TI-Nspire emulator nspire_emu.

Features
========

Ncubate adds the following feature to nspire_emu v030:

GUI:
- NEW: 'File -> Save State' let you save the whole emulator state to a file 
  named <flash_image>.sav. The file is loaded on startup if it exists. You can 
  force a state reload with 'File -> Reload State'.
- CHG: 'File -> Save Flash As...' suggests the current flash image name

Debugger:
- NEW: The register PC can be changed with the command 'rs'. This is useful for 
  example to jump over an halt() call in an Ndless program.

What may be incubated
=====================

- Faster save/reload state
- GDB support (being tested)

Development team
================

Olivier Armand aka ExtendeD  -  < olivier dot calc at gmail dot com >
  based on nspire_emu by Goplat

Changelog
=========

* v030a - 2010/08/15
 - First public release
