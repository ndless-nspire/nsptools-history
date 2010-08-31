#include <os.h>

int main(void) {
	// required because stdout needs the interrupts currently disabled by Ndless
	unsigned intmask = TCT_Local_Control_Interrupts(0);
	puts("hello world!");
	TCT_Local_Control_Interrupts(intmask);
	return 0;
}
