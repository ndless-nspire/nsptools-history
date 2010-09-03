#include <os.h>

char *str;

void hello(void) {
	puts(str);
}
	

int main(void) {
	// required because stdout needs the interrupts currently disabled by Ndless
	unsigned intmask = TCT_Local_Control_Interrupts(0);
	str = "hello world!"; // global variables are supported
	hello();
	TCT_Local_Control_Interrupts(intmask);
	return 0;
}
