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
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include <os.h>

// TODO: use snprintf instead of sprintf

unsigned errcount = 0;

static void error(const char *tstname, const char *errmsg) {
	printf("[%s] FAILED: %s\n", tstname, errmsg);
	errcount++;
}

static void assertUIntEquals(const char *tstname, unsigned expected, unsigned actual) {
	char sbuf[100];
	if (expected != actual) {
		sprintf(sbuf, "%s(%u, %u)", __func__, expected, actual);
		error(tstname, sbuf);
	}
}

static void assertStrEquals(const char *tstname, const char *expected, const char *actual) {
	char sbuf[100];
	if (strcmp(expected, actual)) {
		sprintf(sbuf, "%s(\"%s\", \"%s\")", __func__, expected, actual);
		error(tstname, sbuf);
	}
}

int global_int;
int* nl_relocdata_data[] = {&global_int};

static const unsigned custom_sprintf_addrs[] = {0x102A280C}; // only non-CAS 1.7
#define custom_sprintf SYSCALL_CUSTOM(custom_sprintf_addrs, int __attribute__((__format__(__printf__,2,3))), char *s, const char *format, ...)

int main(int argc, char *argv[]) {
	char buf100[100];
	int ret;

	assertUIntEquals("argc", 1, argc);
	assertStrEquals("argv,strrchr", "ndless_tests.tns", strrchr(argv[0], '/') + 1);
	
	assertUIntEquals("abs,min,max", 4, max(min(abs(-3), 2), 4));
	
	ret = sprintf(buf100, "%i%i%i", 1, 2, 3);
	assertStrEquals("_syscallsvar >4 params", "123", buf100); // tests sprintf. uses _syscallvar_savedlr.
	assertUIntEquals("_syscallsvar return", 3, ret);

	
	if (nl_osvalue((int*)custom_sprintf_addrs, 1)) { // we are on non-CAS 1.7: execute tests which only work on this version.
		unsigned nl_osvalue_data[] = {1, 2, 3};
		assertUIntEquals("nl_osvalue", 1, nl_osvalue((int*)nl_osvalue_data, 3)); // Also tests syscalls extensions
		custom_sprintf(buf100, "%s", "custom");
		assertStrEquals("_syscall_custom", "custom", buf100);
	}
	
	global_int = 1; // tests relocation of global variables 
	nl_relocdata((unsigned*)nl_relocdata_data, 1);
	assertUIntEquals("nl_relocdata", 1, (unsigned)*nl_relocdata_data[0]);
	
	sleep(100);
	
	if (!errcount)
		puts("Successful!");
	else
		printf("%u test(s) failed.\n", errcount);
	exit(0); // tests exit()
}
