Hot rebooting an OS is not possible as is, as some OS global variables have lost their initial values.
The internal memory (0x0 / 0xA4000000, see http://hackspire.unsads.com/wiki/index.php/Virtual_Memory#0x00000000-0x000FFFFF_.281MB.29_-_Internal_RAM )
also have important values required for a stable hot-rebooted OS.

This utility generates patch commands to reset these variables by comparing either:
 - a original decrypted OS image with a post-boot image (0x10000000-0x110F0110, i.e. before the BSS)
 - a pre-boot internal RAM image with a post-boot image (0x0-

nspire_emu's commands from the internal RAM images dump:
- k 10000000
- wm C:\temp\internal-pre-boot.img 0xA4000000 14000
- c (wait for the boot)
- wm C:\temp\internal-post-boot.img 0xA4000000 14000

Comparing an OS image produced by nspire_emu doesn't seem to be enough, the 
post-boot image should be produced from real HW.
