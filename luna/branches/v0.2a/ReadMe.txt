============================
= Luna v0.2a for TI-Nspire =
============================

Luna is a portable command-line converter of Lua programs to TNS (executable)
TI-Nspire documents, compatible with OS 3.0.2.

It can also be used to convert any TI-Nspire problems in XML format to TNS
documents.

Usage
=====

Lua program convertion:    luna [INFILE.lua]  [OUTFILE.tns]
Problem conversion:        luna [Problem.xml] [OUTFILE.tns]

If the input is '-', reads the file from the standard input.

Contact
=======

Oliver Armand aka ExtendeD - olivier.calc@gmail.com
http://ndlessly.wordpress.com/

License
=======

This product includes software developed by the OpenSSL Project for use in the
OpenSSL Toolkit (http://www.openssl.org/).
This product is based on a derived version of MiniZip.
See OpenSSL-License.txt and src/minizip-1.1/MiniZip64_info.txt for more
information.

Building it yourself
====================

On Windows:
* Install OpenSSL: http://www.slproweb.com/products/Win32OpenSSL.html
  Use the package "Win32 OpenSSL v1.0.0d" and its Visual C++ 2008 dependencies.
* Install MinGW
* Copy the content of the include/ directory of OpenSSL to the include/ directory
  of MinGW
* Copy the files lib/MinGW/*.a of OpenSSL to the /lib directory of MinGW
* Rename libeay32.a to libssl.a in the /lib directory of MinGW
* Build and install zlib on MinGW:
  http://kemovitra.blogspot.com/2009/06/mingw-compiling-zlib.html
* 'make dist' Luna

History
=======

2011-09-27: v0.2a
 * FIX: characters at the end of a Lua script might no be correctly XML-escaped
        (thanks Goplat)
 * FIX: wrong buffer size handling might cause '<' and '&' characters in Lua 
        scripts to produce bad TNS files (thanks Goplat)
 * FIX: TNS documents were abnormally big because of trailing garbage data being
        encrypted (thanks Levak)

2011-09-19: v0.2
 * NEW: supports arbitrary TI-Nspire problem conversion, useful to build
        third-party document generators

2011-08-08: v0.1b
 * NEW: lua file can be provided from stdin
 * FIX: crash if input file doesn't exist

2011-08-06: v0.1a
 * Can be built on Linux

2011-08-05: v0.1
 * First release
