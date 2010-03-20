#include "os.h"
#include "arrow.h"
#include <math.h>

#define abs(a) ((a) < 0 ? -(a) : (a))
#define sgn(a) ((a) < 0 ? - 1 : (a) > 0 ? 1 : 0)

static inline void setPixel(int x, int y, int color) {
  unsigned char* p = (unsigned char*)(SCREEN_BASE_ADDRESS + ((x >> 1) + (y << 7) + (y << 5)));
  *p = (x & 1) ? ((*p & 0xF0) | color) : ((*p & 0x0F) | (color << 4));
}

void drawLine(int x1, int y1, int x2, int y2, int color) {
  int u, s, v, d1x, d1y, d2x, d2y, m, n;
  int i;

  u   = x2 - x1;
  v   = y2 - y1;
  d1x = sgn(u);
  d1y = sgn(v);
  d2x = sgn(u);
  d2y = 0;
  m   = abs(u);
  n   = abs(v);
  if (m <= n) {
    d2x = 0;
    d2y = sgn(v);
    m   = abs(v);
    n   = abs(u);
  }
  s = m >> 1;
  for (i = 0; i < m; i++) {
    setPixel(x1, y1, color);
    s += n;
    if (s >= m) {
      s -= m;
      x1 += d1x;
      y1 += d1y;
    }
    else {
      x1 += d2x;
      y1 += d2y;
    }
  }
}

static void arrowVertexes(int x1, int y1, int x2, int y2, int arrow_length, float theta, int* arrow_x1, int* arrow_y1, int* arrow_x2, int* arrow_y2) {
  float angle = atan2f(y2 - y1, x2 - x1) + M_PI;
  *arrow_x1 = x2 + (int)((float)arrow_length * cosf(angle - theta));
  *arrow_y1 = y2 + (int)((float)arrow_length * sinf(angle - theta));
  *arrow_x2 = x2 + (int)((float)arrow_length * cosf(angle + theta));
  *arrow_y2 = y2 + (int)((float)arrow_length * sinf(angle + theta));
}

void arrowTo(int x1, int y1, int x2, int y2, int color) {
  int arrow_x1, arrow_y1;
  int arrow_x2, arrow_y2;

  arrowVertexes(x1, y1, x2, y2, 15, 0.5f, &arrow_x1, &arrow_y1, &arrow_x2, &arrow_y2);
  drawLine(arrow_x1, arrow_y1, x2, y2, color);
  drawLine(arrow_x2, arrow_y2, x2, y2, color);
}

void drawVector(int x1, int y1, int x2, int y2, int color) {
  drawLine(x1, y1, x2, y2, color);
  arrowTo(x1, y1, x2, y2, color);
}
