Hot rebooting an OS is not possible as is, as some OS global variables have lost their initial values.
The internal memory (0x0 / 0xA4000000, see http://hackspire.unsads.com/wiki/index.php/Virtual_Memory#0x00000000-0x000FFFFF_.281MB.29_-_Internal_RAM )
also have important values required for a stable hot-rebooted OS.

This utility generates patch commands to reset these variables by comparing either:
 - a original decrypted OS image with a post-boot image (0x10000000-end_of_os, i.e. before the BSS)
 - a pre-boot internal RAM image with a post-boot image (0x0-0x14000)

nspire_emu's commands for the OS and internal RAM images dump:
wm C:\temp\internal-post-boot.img 0xA4000000 14000
wm C:\temp\os-post-boot.img 0x10000000 [end_of_os-0x10000000]
k 10000000
c
[reset]
wm C:\temp\internal-pre-boot.img 0xA4000000 14000
wm C:\temp\os-pre-boot.img 0x10000000 [end_of_os-0x10000000]

end_of_os is just before the BSS.

Then:
- MakeHotRebootPtch.exe 0x0 internal-pre-boot.img internal-post-boot.img hrpatches-internal-ram-ncas-3.6.0.h
Remove the first patches for the vectors and the screen buffer (0-96FF).
- MakeHotRebootPtch.exe 0x10000000 os-pre-boot.img os-post-boot.img hrpatches-os-ncas-3.6.0.h

Comparing an OS image produced by nspire_emu doesn't seem to be enough, the 
post-boot image should be produced from real HW.
