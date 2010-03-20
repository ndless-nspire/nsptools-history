#include <os.h>
#include "arrow.h"

asm(".string \"PRG\"\n");

int main(void) {
  memset(SCREEN_BASE_ADDRESS, 0xFF, SCREEN_BYTES_SIZE);

  drawVector(0, 0, 258, 67, 0x00);
  while (!isKeyPressed(KEY_NSPIRE_ESC));

  return 0;
}
