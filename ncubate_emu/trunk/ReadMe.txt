===============
= Ncubate Emu =
===============

What is it?
===========

Ncubate Emu is an enhanced of the TI-Nspire emulator nspire_emu.

Features
========

Ncubate adds the following features to nspire_emu v032:

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
- NEW: Command 'ss' to search a string in memory.
- NEW: "+x" is the default flag for the breakpoint command 'k' 
- NEW: Command 'wf' to load a file to memory.
- CHG: The breakpoint command 'k' takes as parameter a virtual address instead 
  of a physical address.
- CHG: Debugger command 'w' renamed to 'wm'
- CHG: The command 'wm' takes as parameter a virtual address instead of a 
  physicall address.
- FIX: Command 'd' doesn't crash if the parameter is missing
- FIX: Broken 'w' command

CPU/Ports:
- FIX: no more 'bad read word' on auto power down, but still hangs

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

* v032b - <not released yet>
 - FIX: 'j' (over a non-branch instruction) and 'rs pc' debugger commands 
   incorrectly skipped the target instruction
 - FIX: no more 'bad read word' on auto power down, but still hangs
 - FIX: ineffective command line option /D when saved state was reloaded on startup

* v032a - 2010/09/21
 - Migrate to nspire_emu v032
 - ADD: Debugger command 'ss'
 - ADD: Debugger command 'wf'
 - CHG: Debugger command 'w' renamed to 'wm'
 - FIX: Debugger command 'j' supports thumb state
 - FIX: Broken debugger command 'w' (now 'wm')

* v031a - 2010/08/30
 - Migrate to nspire_emu v031
 - ADD: State file version tracking
 - ADD: Link target folder saved to save files
 - ADD: Debugger command 'j'
 - ADD: Debugger command 'ww'
 - ADD: "+x" is the default flag for command 'k' 
 - CHG: Command 'k' uses virtual addresses
 - CHG: Add shortcut for link connect, change shortcut to send a document
 - CHG: Remove log "Wrote to translated code..."
 - FIX: Couldn't save flash after sending a document from another directory when 
   loading a flash image with a relative path.
 - FIX: Command 'd' doesn't crash if the parameter is missing
 - FIX: "disconnect" not required anymore after a state reload before connecting

* v030a - 2010/08/15
 - First public release
