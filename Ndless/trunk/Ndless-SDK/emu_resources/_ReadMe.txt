To be able to use the 'nspire_emu' TI-Nspire emulator integration with the 
NdlessEditor, you must first drop several resources to this directory and let 
the OS image install itself to a NAND image.

1) Download OS "TI-Nspire CX (v 3.1)" or "TI-Nspire CX CAS (v 3.1)" from 
   http://tiplanet.org/forum/archives_list.php?id=OS+Nspire
   Emulation of non-CX devices is not supported.

2) Drop it in this directory [emu_resources]

3) Use PolyDumper to dump your TI-Nspire CX boot1 and boot2 images:
   a) Install Ndless on your TI-Nspire CX
   b) Transfer the file emu_resources/polydumper/polydumper_3.1.tns to your
      TI-Nspire CX
   c) Run polydumper_3.1.tns

4) Transfer the files boot1.img.tns and boot2.img.tns produced by PolyDumper 
   from your TI-Nspire CX to the directory emu_resources/ on the computer side

5) Set up the NAND image from the NdlessEditor with Tools > 'Run the TI-
   Nspire emulator'. Let the OS reboot, then press 'I' when asked. The OS will 
   install and boot up.
   
6) Install Ndless:
   Create a new folder called 'ndless' in 'My Documents'.
   Set the target folder with 'Link > Set Target Folder...' to 'ndless'.
   Transfer ndless_resources.tns with 'Link > Connect' then
   'Link > Send Document...'.
   Transfer 'ndless_installer-3.1.0.tco|.tcc' with 'Link > Connect' then
   'Link > Send OS...'.
   A popup should confirm the installation.

7) Save the NAND (flash) image with the File > 'Save Flash As...' option of 
   nspire_emu to a file named 'nand.img' in this directory.
   The image will be loaded the next time you launch the emulator from the 
   NdlessEditor (Tools > TI-Nspire emulator).
