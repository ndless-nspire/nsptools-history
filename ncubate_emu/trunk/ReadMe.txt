===============
= Ncubate Emu =
===============

What is it?
===========

Ncubate Emu is an enhanced of the TI-Nspire emulator nspire_emu.

Features
========

Ncubate adds the following features to nspire_emu v031:

GUI:
- NEW: 'File -> Save State' let you save the whole emulator state to a file 
  named <flash_image>.sav. The file is loaded on startup if it exists. You can 
  request a state reload with 'File -> Reload State'.
  Caution, newer versions of Ncubate may fail to read .sav files produced by
  previous versions. Delete these files when upgrading.
- NEW: 'File -> Save Flash As...' suggests the current flash image name
- NEW: several keyboard shortcuts added

Debugger:
- NEW: The register PC can be changed with the command 'rs'. 
- NEW: Command 'j' to jump to the next instruction. This is useful for example 
  to jump over an halt() call in an Ndless program.
- NEW: Command 'ww' to write a word to an address.
- CHG: The breakpoint command 'k' takes as parameter a virtual address instead 
  of a physical address.
- FIX: Command 'd' doesn't crash if the parameter is missing

Flash:
- FIX: Flash image closed when synced to disk, for external edition

Build:
- NEW: Makefile.config for custom compilation flags
- NEW: make clean

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

* v031a - unreleased
 - Migrate to nspire_emu v031
 - ADD: Debugger command 'j'
 - ADD: Debugger command 'ww'
 - CHG: Debugger command 'k' uses virtual addresses
 - FIX: Couldn't save flash after sending a document from another directory when 
   loading a flash image with a relative path.
 - FIX: Command 'd' doesn't crash if the parameter is missing

* v030a - 2010/08/15
 - First public release
