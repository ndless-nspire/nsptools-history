#ifndef _UTILS_H_
#define _UTILS_H_

#define SCREEN_BASE_ADDRESS     (void*)0xA4000100
#define SCREEN_BYTES_SIZE       38400
#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240
#define KEY_MAP                 (void*)0x900E0000

#define KEY_NSPIRE_RET          (t_key){0x10, 0x01}
#define KEY_NSPIRE_ENTER        (t_key){0x10, 0x02}
#define KEY_NSPIRE_PLUS         (t_key){0x12, 0x02}
#define KEY_NSPIRE_MINUS        (t_key){0x14, 0x02}
#define KEY_NSPIRE_MULTIPLY     (t_key){0x16, 0x02}
#define KEY_NSPIRE_DIVIDE       (t_key){0x18, 0x02}
#define KEY_NSPIRE_ESC          (t_key){0x1C, 0x80}
#define KEY_NSPIRE_C            (t_key){0x1C, 0x04}


#define BLACK                   0x0
#define WHITE                   0xF

#define WAIT(n, name) \
  asm ( \
    "ldmfd sp!, {r0}\n" \
    "ldr r0, ="n"\n" \
    "_loop_"name":\n" \
    "cmp r0, #0\n" \
    "sub r0, #1\n" \
    "bne _loop_"name"\n" \
    "stmfd sp!, {r0}\n" \
  );

typedef struct {
  int row;
  int col;
} t_key;

typedef struct {
  int x, y, w, h;
} t_rect;

typedef struct {
  float x, y;
} t_point;

typedef enum {false, true} bool;

extern inline bool rect_intersect(const t_rect* r1, const t_rect* r2);
extern void clearScreen();
extern inline int getPixel(int x, int y);
extern inline void setPixel(int x, int y, int color);
extern inline int isKeyPressed(t_key key);
extern int rand();
extern void showSimpleDialogBox(const char* title, const char* msg);
extern float sqrtf(float num);
extern void fondu(int n);

#endif
