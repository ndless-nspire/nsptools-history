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

static unsigned const custom_sprintf_addrs[] = {0x102A280C, 0};
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

	unsigned nl_osvalue_data[] = {1, 2, 3};
	assertUIntEquals("nl_osvalue", 1, nl_osvalue((int*)nl_osvalue_data, 3)); // also tests syscalls extensions. must be run on OS 1.7
	
	global_int = 1; // tests relocation of global variables 
	nl_relocdata((unsigned*)nl_relocdata_data, 1);
	assertUIntEquals("nl_relocdata", 1, (unsigned)*nl_relocdata_data[0]);
	
	custom_sprintf(buf100, "%s", "custom");
	assertStrEquals("_syscall_custom", "custom", buf100);
	
	if (!errcount)
		puts("Successful!");
	else
		printf("%u test(s) failed.\n", errcount);
	exit(0); // tests exit()
}
