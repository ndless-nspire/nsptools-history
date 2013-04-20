==============================
= HIDn alpha 1 for TI-Nspire =
==============================

HIDn is a Ndless-based set of HID USB device drivers for the TI-Nspire.
HIDn let you use USB keyboard and mouse to interact with the TI-Nspire OS.

Prerequisites
=============

* Ndless v3.1-r758 or higher installed. Get it here: http://ndlessly.wordpress.com/
* A $5 USB A female to mini adapter: http://www.amazon.com/s/?field-keywords=usb%20mini%20adapter

Usage
=====

Use hidn-azerty.tns if you have an AZERTY keyboard. Run 'hidn' or 'hidn-azerty' 
to install the drivers. Then plug in your adapter and our USB device.
You don't need to reinstall HIDn as long as the calculator doesn't reboot.
You can transfer HIDn to /ndless/startup for silent automatic installation on 
startup.

Troubleshooting
===============

* To make sure your USB adapter is supported by the TI-Nspire hardware, plug it 
  in with the USB device *without* installing HIDn. The following OS message 
  should be displayed: "Cannot communicate with the peripheral attached to 
  handheld".

* Some keyboard or mouse may not be supported

Not implemented yet
===================

Both:

 * No unistallation. To uninstall it, reboot the calculator. To upgrade the 
   drivers, you can run HIDn again without uninstallation, but there will be a 
   slight memory leak.
 * USB hubs are not supported. Using one will make the TI-Nspire OS reboot.
 * No integration with Ndless-based programs.

Mouse:

 * The current coordinates are not shared with the OS pad

Keyboard:

 * CAPS not supported
 * No key repeat for most keys
 * Partial key mapping
 * Modifiers (shift, alt) are not supported except for upper case
 * No LED control

Contact
=======

Oliver Armand aka ExtendeD - olivier.calc@gmail.com
http://ndlessly.wordpress.com/

History
=======

2013-04-20: alpha 1
 * First public alpha release
 