#define UNLOADED_FILE   1
#include <idc.idc>

// This is a template file, %%tokens%% are replaced
static main(void)
{
	auto fp;
	fp = fopen("%%idcfile%%", "wb");
	GenerateFile(OFILE_IDC, fp, 0, 0x15000000, 0);
	fclose(fp);
	Exit(0);
}
