#include <os.h>
#include "screen.h"
#include "console.h"
#include "config.h"

#define NAND_PAGE_SIZE 512
#ifdef NDLESS31
	#define BOOT2_NAND_OFFSET is_cx?(0x0100 * NAND_PAGE_SIZE):(0x0020 * NAND_PAGE_SIZE)
	#define DIAGS_NAND_OFFSET is_cx?(0x1900 * NAND_PAGE_SIZE):(0x0B00 * NAND_PAGE_SIZE)
#else
	#define BOOT2_NAND_OFFSET (0x0020 * NAND_PAGE_SIZE)
	#define DIAGS_NAND_OFFSET (0x0B00 * NAND_PAGE_SIZE)
#endif
#define BOOT2_PAGES_NUM 0x0A60
#define BOOT2_SIZE (BOOT2_PAGES_NUM * NAND_PAGE_SIZE)
#define DIAGS_PAGES_NUM 0x0500
#define DIAGS_SIZE (DIAGS_PAGES_NUM * NAND_PAGE_SIZE)

#ifndef HAS_NAND
// definitions below are needed for Ndless 1.0/1.1/1.1.1 (OSes 1.1.9253 / 1.1.9170)
	#ifdef NDLESS11
		#ifdef CAS
			#define read_nand_ 0x1015F3D0
		#else
			#define read_nand_ 0x1015F3A0
		#endif
	#endif
	#ifdef NDLESS17
		static const unsigned read_nand_addrs[] = {0x102261a4,0x102281b4};  //  ncas1.7, cas1.7
		#define read_nand SYSCALL_CUSTOM(read_nand_addrs ,void,  void* dest, int size, int offset, int, int percent_max, void *progress_cb )		
	#endif
	#ifdef NDLESS20
		static const unsigned read_nand_addrs[] = {0x102261a4,0x102281b4,0x10276478,0x10276d48,0x1028a08c,0x1028a95c};  //  ncas1.7, cas1.7, ncas2.0.1, cas2.0.1, ncas2.1.0, car2.1.0
		#define read_nand SYSCALL_CUSTOM(read_nand_addrs ,void,  void* dest, int size, int offset, int, int percent_max, void *progress_cb )		
	#endif
	#ifdef NDLESS31
		static const unsigned read_nand_addrs[] = {0X10071F5C,0x10071EC4,0x10071658, 0X100715E8};  //  ncas3.1.0, cas3.1.0, OS_NCASCX3.1.0, OS_CASCX3.1.0
		#define read_nand SYSCALL_CUSTOM(read_nand_addrs ,void,  void* dest, int size, int offset, int, int percent_max, void *progress_cb )
	#endif
#endif

#if defined(NDLESS11) || defined(NDLESS12)
asm(".string \"PRG\"\n");
#define read_nand (_oscall(void, read_nand_, void* dest, int size, int offset, int, int percent_max, void *progress_cb))
#endif

extern unsigned char back[320*240*3];
unsigned char sscreen[320*240*2];

int main(int argc, char** argv) {
	char* path = argv[0];
	int i = strlen(path)-1;
	while(path[i]!='/')
		i--;
	path[i+1]=0;
	initScr();
	char filepath[512];
	strcpy(filepath,path);
	char* filename=filepath+i+1;
	clrScr();
	resetConsole();
	int size = 320*240/2;
	if(is_cx) size=320*240*2;
	dispBufImgRGB(sscreen,0,0,back,320,240);
	memcpy(SCREEN_BASE_ADDR,sscreen,size);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	displn("",0,1);
	disp("Dumping Boot1... ",0,1);
	strcpy(filename,"boot1.img.tns");
	FILE *ofile = fopen(filepath, "wb");
	if(ofile)
	{	asm("ldr r1,=0x00000C12");
		asm("mrc p15,0,r0,c2,c0,0");
		asm("str r1,[r0,#4]");
		asm("ldr r0,=0x00100000");
		asm("mcr p15,0,r0,c8,c7,1");
		if(512*1024==fwrite((volatile void*)0x00100000, 1, 512*1024, ofile))
			displn("Done!",0,1);
		else
			displn("Error writing file...",0,1);
		fclose(ofile);

	}
	else
		displn("Error creating file...",0,1);
      void *buf = malloc(BOOT2_SIZE);

#ifndef HAS_NO_NAND
	disp("Dumping Diags... ",0,1);
	strcpy(filename,"diags.img.tns");
	ofile = fopen(filepath, "wb");
	if(ofile)
	{	read_nand(buf, DIAGS_SIZE, DIAGS_NAND_OFFSET, 0, 0, NULL);
		if(DIAGS_SIZE==fwrite(buf, 1, DIAGS_SIZE, ofile))
			displn("Done!",0,1);
		else
			displn("Error writing file...",0,1);
		fclose(ofile);
	}
	else
		displn("Error creating file...",0,1);

	disp("Dumping Boot2... ",0,1);
	strcpy(filename,"boot2.img.tns");
	ofile = fopen(filepath, "wb");
	if(ofile)
	{	read_nand(buf, BOOT2_SIZE, BOOT2_NAND_OFFSET, 0, 0, NULL);
		if(BOOT2_SIZE==fwrite(buf, 1, BOOT2_SIZE, ofile))
			displn("Done!",0,1);
		else
			displn("Error writing file...",0,1);
		fclose(ofile);
	}
	else
		displn("Error creating file...",0,1);

#endif
	disp("Dumping OSyst... ",0,1);
	strcpy(filename,"os.img.tns");
	ofile = fopen(filepath, "wb");
	if(ofile)
	{	FILE* ifile = fopen("/phoenix/install/TI-Nspire.tnc", "rb");
		if(ifile)
		{	int error=0;
			while( (i = fread(buf, 1, BOOT2_SIZE, ifile)) && !error )
			{	if(i!=fwrite(buf, 1, i, ofile))
					error=1;
			}
			if(!error)
				displn("Done!",0,1);
			else
				displn("Error writing file...",0,1);
			fclose(ifile);
		}
		else
			displn("Error opening file...",0,1);
		fclose(ofile);
	}
	else
		displn("Error creating file...",0,1);
	free(buf);

	displn("",0,1);
	displn("",0,1);
	pause("",0,1);
	return 0;
}
