/****************************************************************************
 * Automated test cases
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010-2012
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#ifndef _TINSPIRE
#error _TINSPIRE not defined as it should be
#endif

#include <os.h>
#include "ndless_tests.h"
#include "../ndless.h"

// TODO: use snprintf instead of sprintf

int global_int;
int* nl_relocdata_data[] = {&global_int};

static const unsigned custom_sprintf_addrs[] = {0x10376F28}; // only non-CAS 3.1
#define custom_sprintf SYSCALL_CUSTOM(custom_sprintf_addrs, int __attribute__((__format__(__printf__,2,3))), char *s, const char *format, ...)

/* the actual parameters should be: dummy, (char)1, char(0x20) */
static void test_va(char __attribute__((unused)) dummy, ...) {
	char buf[10];
	va_list vl;
	va_start(vl, dummy);
	unsigned char c = va_arg(vl, int);
	assertUIntEquals("va_arg", 1, c);
	vsprintf(buf, "%c", vl);
	assertStrEquals("vsprintf", " ", buf);
	va_end(vl);
}

int main(int argc, char *argv[]) {
	char buf[100];
	char buf2[100];
	int ret;
	char *ptr;
	FILE *file;
	struct stat sstat;
	DSTAT dstat;

	assertUIntEquals("TCT_Local_Control_Interrupts", 0xFFFFFFFF, TCT_Local_Control_Interrupts(0));
	assertUIntEquals("argc", 2, argc);
	assertStrEquals("argv0", "ndless_tests.test.tns", strrchr(argv[0], '/') + 1);
	assertStrEquals("argv1", "ndless_tests.test.tns", strrchr(argv[1], '/') + 1); // file association (run with ourself)
		
	ret = sprintf(buf, "%i%i%i", 1, 2, 3);
	assertStrEquals("_syscallsvar >4 params", "123", buf); // tests sprintf. uses _syscallvar_savedlr.
	assertUIntEquals("_syscallsvar return", 3, ret);
	
	if (nl_osvalue((int*)custom_sprintf_addrs, 1)) { // we are on non-CAS 3.1: execute tests which only work on this version.
		unsigned nl_osvalue_data[] = {1, 2, 3};
		assertUIntEquals("nl_osvalue", 1, nl_osvalue((int*)nl_osvalue_data, 3)); // Also tests syscalls extensions
		custom_sprintf(buf, "%s", "custom");
		assertStrEquals("_syscall_custom", "custom", buf);
	}
	
	global_int = 1; // tests relocation of global variables 
	nl_relocdata((unsigned*)nl_relocdata_data, 1);
	assertUIntEquals("nl_relocdata", 1, (unsigned)*nl_relocdata_data[0]);
	
	nl_set_resident(); // caution, ;will leak. This at least checks that it doesn't crash.
	
	/* syscalls */
	buf[0] = 1; buf[1] = 2; buf[2] = 3; buf[3] = 4; buf[4] = 5;
	assertUIntEquals("read_unaligned_longword", 0x05040302, read_unaligned_longword(buf + 1));
	assertUIntEquals("read_unaligned_word", 0x0302, read_unaligned_word(buf + 1));
	ascii2utf16(buf, "abc", sizeof(buf));
	assertUIntEquals("ascii2utf16", 'a', buf[0]);
	assertUIntEquals("utf16_strlen", 3, utf16_strlen(buf));
	utf162ascii(buf2, buf, sizeof(buf2));
	assertStrEquals("utf162ascii", "abc", buf2);
	
	assertUIntEquals("isalpha", TRUE, isalpha('a'));
	assertUIntEquals("isascii", TRUE, isascii('+'));
	assertUIntEquals("isdigit", TRUE, isdigit('0'));
	assertUIntEquals("islower", TRUE, islower('a'));
	assertUIntEquals("isprint", TRUE, isprint('a'));
	assertUIntEquals("isspace", TRUE, isspace(' '));
	assertUIntEquals("isupper", TRUE, isupper('A'));
	assertUIntEquals("isxdigit", TRUE, isxdigit('f'));
	assertUIntEquals("tolower", 'a', tolower('A'));
	assertUIntEquals("toupper", 'A', toupper('a'));
	assertUIntEquals("atoi", 1, atoi("1"));
	// assertDblEquals("atof", 1.1, atof("1.1")); // TODO fails
	// strtod TODO fails
	
	ptr = malloc(100);
	assertNotNull("malloc", ptr);
	free(ptr);
	ptr = calloc(5, 4);
	assertNotNull("calloc", ptr);
	assertZero("calloc[0]", *(char*)ptr);
	ptr = realloc(ptr, 100);
	assertNotNull("realloc", ptr);
	free(ptr);
	
	memset(buf, 1, sizeof(buf));
	assertUIntEquals("memset", 1, buf[10]);
	buf[0] = 0; buf[10] = 1;
	memcpy(buf, buf + 10, 1);
	assertUIntEquals("memcpy", 1, buf[0]);
	buf[0] = 0; buf[10] = 1;
	memmove(buf, buf + 10, 1);
	assertUIntEquals("memmove", 1, buf[0]);
	buf[0] = 1; buf[10] = 1;
	assertZero("memcmp", memcmp(buf, buf + 10, 1));
	buf[0] = 0; ; buf[10] = 1;
	memrev(buf, 11);
	assertUIntEquals("memrev", 1, buf[0]);
	assertUIntEquals("strlen", 3, strlen("abc"));
	strcpy(buf, "abc");
	assertZero("strcpy,strcmp", strcmp(buf, "abc"));
	buf[2] = 0;
	strncpy(buf, "abc", 2);
	assertZero("strncpy", strcmp(buf, "ab"));
	strcpy(buf, "abc");
	assertZero("strncmp", strncmp(buf, "a", 1));
	strcpy(buf, "a");
	strcat(buf, "bc");
	assertZero("strcat", strcmp(buf, "abc"));
	strcpy(buf, "a");
	strncat(buf, "bc", 1);
	assertZero("strncat", strcmp(buf, "ab"));
	ptr = strchr("abc", 'b');
	assertUIntEquals("strchr", 'b', *ptr);
	ptr = strrchr("abc", 'a');
	assertUIntEquals("strrchr", 'a', *ptr);
	ptr = strpbrk("abc", "dc");
	assertUIntEquals("strpbrk", 'c', *ptr);
	assertUIntEquals("strcspn", 3, strcspn("123abc", "abc"));
	assertUIntEquals("strspn", 3, strspn("abcdef", "abc"));
	file = fopen("unexist.ent", "r");
  assertStrEquals("strerror,errno", "No Such File Or Directory", strerror(errno));
	assertIntEquals("strtol", -1, strtol("-1", NULL, 10));
	assertUIntEquals("strtoul", 0xA0000000, strtoul("0xA0000000", NULL, 16));
	assertStrEquals("strstr", "def", strstr("abcdef", "def"));
	
	test_va(0, (char)1, (char)0x20);
	sprintf(buf, "%s", "abc");
	assertStrEquals("sprintf", "abc", buf);
	
	srand(5050);
	assertUIntEquals("rand-1", 0x6A5, rand());
	assertUIntEquals("rand-2", 0x6B5D, rand());
	srand(1818);
	assertUIntEquals("rand-2", 0x264, rand());
	
	strncpy(buf, argv[0], sizeof(buf));
	*(strrchr(buf, '/') + 1) = '\0'; // keep the folder
	strncat(buf, "__testfile.tns", sizeof(buf)); // buf = temp file path
	file = fopen(buf, "wb+");
	assertNotNull("fopen", file);
	file = freopen(buf, "wb+", file);
	assertNotNull("freopen", file);
	assertUIntEquals("fwrite", 4, fwrite("abc", 1, 4, file));
	assertZero("fflush", fflush(file));
	rewind(file);
	assertUIntEquals("fread-1", 4, fread(buf2, 1, 4, file));
	assertStrEquals("fread-2", "abc", buf2);
	assertZero("fseek", fseek(file, 0, SEEK_SET));
	assertUIntEquals("fprintf", 3, fprintf(file, "%s", "abc"));
	rewind(file);
	assertUIntEquals("fputc", 'a', fputc('a', file));
	fseek(file, -1, SEEK_CUR);
	assertUIntEquals("fgetc", 'a', fgetc(file));
	rewind(file);
	assertUIntEquals("getc", 'a', getc(file));
	rewind(file);
	assertNonZero("fputs", fputs("abc\ndef", file));
	rewind(file);
	assertStrEquals("fgets", "abc\n", fgets(buf2, 10, file));
	rewind(file);
	fputc('a', file);
	rewind(file);
	assertUIntEquals("ungetc-1", 'a', ungetc(fgetc(file), file));
	assertUIntEquals("ungetc-2", 'a', fgetc(file));
	assertZero("fclose", fclose(file));
	assertZero("truncate", truncate(buf, 2));
	assertZero("stat", stat(buf, &sstat));
	assertIntEquals("stat-truncate", sstat.st_size, 2);
	
	assertZero("unlink", unlink(buf));
	file = fopen(buf, "wb+");
	assertNonZero("feof-1", feof(file));
	fputc('a', file);
	rewind(file);
	assertZero("feof-2", feof(file));
	fclose(file);
	file = fopen(buf, "wb");
	fread(buf2, 1, 1, file);
	assertNonZero("ferror", ferror(file));
	fclose(file);
	assertZero("remove", remove(buf));
	assertZero("mkdir", mkdir("/tmp/__testdir", 0));
	assertZero("rename", rename("/tmp/__testdir", "/tmp/__testdir2"));
	assertZero("chdir", chdir("/tmp"));
	assertZero("rmdir", rmdir("__testdir2"));
	
	assertZero("NU_Set_Current_Dir", NU_Set_Current_Dir("A:\\tmp"));
	assertZero("NU_Current_Dir-1", NU_Current_Dir("A:", buf));
	assertStrEquals("NU_Current_Dir-2", "\\tmp\\", buf);
	assertZero("NU_Get_First", NU_Get_First(&dstat, "A:\\*.*"));
	assertZero("NU_Get_Next-1", NU_Get_Next(&dstat));
	assertStrEquals("NU_Get_Next-2", "tmp", dstat.filepath);
	NU_Done(&dstat);
	
	DIR	*dp;
	struct dirent	*ep;		 
	assertNotNull("opendir", (dp = opendir("/")));
	assertNotNull("readdir-1", (ep = readdir(dp)));
	assertNotNull("readdir-2", (ep = readdir(dp)));
	assertStrEquals("readdir.d_name",	"tmp", ep->d_name);
	assertZero("closedir", closedir(dp));
	
	assertUIntLower("keypad_type", 5, *keypad_type);
	assertNonZero("keypad_type", *keypad_type);
	
	/* libndls */
	assertUIntEquals("isalnum", TRUE, isalnum('0'));
	assertUIntEquals("iscntrl", TRUE, iscntrl('\0'));
	assertUIntEquals("abs,min,max", 4, max(min(abs(-3), 2), 4));
	assertUIntEquals("bswap16", 0x0100, bswap16(0x0001));
	assertUIntEquals("bswap32", 0x03020100, bswap32(0x00010203));
	
	assertTrue("is_cx", (is_cx && *(unsigned*)0x900A0000 == 0x101) || (!is_cx && *(unsigned*)0x900A0000 == 0x01000010));
	assertTrue("is_touchpad", (*keypad_type != 3  &&  *keypad_type != 4) || is_touchpad);
	assertTrue("isstartup", nl_isstartup()); // will only pass if run as startup program
	sleep(100);
	
	/* Ndless internals */
	char * cfg_path = "/tmp/ndless_tests.cfg";
	FILE *cfg_file = fopen(cfg_path, "wb");
	fputs("# comment\n", cfg_file);
	fputs("   key  = value # comment\n", cfg_file);
	fputs("\n", cfg_file);
	fputs("   windows=win\r\n", cfg_file);
	fputs("empty=\n", cfg_file);
	fputs("dummy", cfg_file);
	fclose(cfg_file);
	cfg_open_file(cfg_path);
	assertStrEquals("cfg_get_key", "value", cfg_get("key"));
	assertStrEquals("cfg_get_win", "win", cfg_get("windows"));
	assertStrEquals("cfg_get_empty", "", cfg_get("empty"));
	assertNull("cfg_get_dummy", cfg_get("dummy"));
	cfg_close();
	remove(cfg_path);
	
	if (!errcount) {
		fputc('S', stdout);
		puts("uccessful!");
	}
	else
		printf("%u test(s) failed.\n", errcount);
	exit(0); // tests exit()
}
