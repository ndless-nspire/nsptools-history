#include <os.h>

int main(void) {
	// required because stdout needs the interrupts currently disabled by Ndless
	puts("hello world!");
	clrscr();
	wait_key_pressed();
	puts("done");
	memset(SCREEN_BASE_ADDRESS, 0, SCREEN_BYTES_SIZE);
	wait_key_pressed();
	return 0;
}
