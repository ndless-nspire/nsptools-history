==============================
= TI-Nspire On-Calc Debugger =
==============================

What is it?
===========

OCD is a static library for native (Ndless) TI-Nspire developers. It should be 
used for low-level (assembly) debugging of programs for which traditional 
techniques (emulator or printf based) don't suit, for instance when interacting 
with hardware I/O ports.

It displays on the calculator screen a command line interface that mirrors most 
of the debugger commands of Goplat's nspire_emu emulator.

How do I use it?
================

 1) Link the program to debug with libocd
 ----------------------------------------
 
Edit the program's Makefile with:
GCCFLAGS=-I ".../path/to/ocd"
LDFLAGS=-L ".../path/to/ocd" -locd

 2) Enable the debugger
 ----------------------

Include the header file: #include <ocd.h>
At the beginning of the program to debug, call ocd_init().
At the end of the program to debug, call ocd_cleanup().
Set a breakpoint with ocd_break(), or at a given address with 
ocd_set_breakpoint() (see ocd.h).
The debugger console will show up when a breakpoint is hit.
The supported debugger commands can be displayed with the command 'h' or '?'.

Known issues and limitations
============================

* Some syscall stubs such as printf are reentrant but are used by ocd.
 Don't try to step into such stubs.

Many thanks to
==============

Goplat for nspire_emu - some parts of the original code have been reused.
compu for the nice nspireio library.

Author
======

  Olivier Armand aka ExtendeD  -  < olivier dot calc at gmail dot com >
  http://ndlessly.wordpress.com/

Changelog
=========

xxxx - v0.1: initial version
