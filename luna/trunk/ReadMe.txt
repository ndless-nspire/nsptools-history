============================
= Luna v0.1a for TI-Nspire =
============================

Luna is a portable command-line converter of Lua programs to TNS (executable)
TI-Nspire documents, compatible with OS 3.0.2.

Usage
=====

luna [INFILE.lua] [OUTFILE.tns]

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

2011-08-0: v0.1a
 * Can be built on Linux

2011-08-05: v0.1
 * First release
