#include "os.h"
#include "utils.h"

bool rect_intersect(const t_rect* r1, const t_rect* r2) {
  if ((r1->y + r1->h) < r2->y) return false;
  if (r1->y > (r2->y + r2->h)) return false;
  if ((r1->x + r1->w) < r2->x) return false;
  if (r1->x > (r2->x + r2->w)) return false;
  return true;
}

void clearScreen() {
  memset(SCREEN_BASE_ADDRESS, 0xFF, SCREEN_BYTES_SIZE);
}

void setPixel(int x, int y, int color) {
  unsigned char* p = (unsigned char*)(SCREEN_BASE_ADDRESS + ((x >> 1) + (y * SCREEN_WIDTH >> 1)));
  *p = (x & 1) ? ((*p & 0xF0) | color) : ((*p & 0x0F) | (color << 4));
}

int getPixel(int x, int y) {
  unsigned char* p = (unsigned char*)(SCREEN_BASE_ADDRESS + ((x >> 1) + (y * SCREEN_WIDTH >> 1)));
  return ((x & 1) ? (*p & 0x0F) : (*p >> 4));
}

int isKeyPressed(t_key key) {
  return !((*(short*)(KEY_MAP + key.row)) & key.col);
}

int rand() {
  static int m_w = 56789;
  static int m_z = 38765;

  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

void showSimpleDialogBox(const char* title, const char* msg) {
  char* buffTitle;
  char* buffMsg;

  buffTitle = (char*)malloc(0x400);
  buffMsg = (char*)malloc(0x400);

  ascii2utf16(buffTitle, title, 0x100);
  ascii2utf16(buffMsg, msg, 0x100);
  show_dialog_box2(0, buffTitle, buffMsg);

  free(buffTitle);
  free(buffMsg);
}

float sqrtf(float num) {
  float mod = 1;
  float c = 0;
  int d;

  for (d = 0; d < 50; c += mod, d++)
  if((c * c) > num) {
    c -= mod;
    mod /= 10;
  }

  return c;
}

void fondu(int n) {
  int i, color1, color2;
  unsigned char* p = (unsigned char*)SCREEN_BASE_ADDRESS;

  for (i = 1; i < SCREEN_BYTES_SIZE;  ++i) {
    color1 = (*p & 0x0F) + n;
    color2 = (*p >> 4) + n;
    if (color1 > 0xF) color1 = 0xF;
    if (color2 > 0xF) color2 = 0xF;
    if (color1 < 0) color1 = 0;
    if (color2 < 0) color2 = 0;

    *p = (color2 << 4) | (color1 & 0xF);
    p++;
  }
}
