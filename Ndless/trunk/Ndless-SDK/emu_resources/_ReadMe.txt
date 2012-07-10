To be able to use the 'nspire_emu' TI-Nspire emulator integration with the 
NdlessEditor, you must first drop several resources to this directory and let 
the OS image install itself to a NAND image.

1) Download OS "TI-Nspire CX (v 3.1)" or "TI-Nspire CX CAS (v 3.1)" from 
   http://tiplanet.org/forum/archives_list.php?id=OS+Nspire

2) Drop it in this directory [emu_resources]

3) Use PolyDumper (provided in [emu_resources]) to dump your TI-Nspire CX boot1 
   and boot2 images.

4) Transfer boot1.img.tns and boot2.img.tns produced by PolyDumper to this 
   directory [emu_resources/]

5) Set up the NAND image from the NdlessEditor with Tools > 'Run the TI-
   Nspire emulator'. Let the OS reboot, then press 'I' when asked. The OS will 
   install and boot up.

6) Save the NAND (flash) image with the File > 'Save Flash As...' option of 
   nspire_emu to a file named 'nand.img' in this directory.
   The image will be loaded the next time you launch the emulator from the 
   NdlessEditor.
