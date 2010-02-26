typedef struct{} _ssize_t;
struct _reent{};
struct stat{};
typedef struct{} _off_t;
typedef struct{} ptrdiff_t;
typedef unsigned long size_t;

_ssize_t _read_r(__attribute__((unused)) struct _reent *r, __attribute__((unused)) int file, __attribute__((unused)) void *ptr, __attribute__((unused)) size_t len) {
  return (_ssize_t){};
}

_ssize_t _write_r(__attribute__((unused)) struct _reent *r, __attribute__((unused)) int file, __attribute__((unused)) const void *ptr, __attribute__((unused)) size_t len) {
  return (_ssize_t){};
}

int _close_r(__attribute__((unused)) struct _reent *r, __attribute__((unused)) int file) {
  return 0;
}

_off_t _lseek_r(__attribute__((unused)) struct _reent *r, __attribute__((unused)) int file, __attribute__((unused)) _off_t ptr, __attribute__((unused)) int dir) {
  return (_off_t){};
}

int _fstat_r(__attribute__((unused)) struct _reent *r, __attribute__((unused)) int file, __attribute__((unused)) struct stat *st) {
  return 0;
}

void* _sbrk_r(__attribute__((unused)) struct _reent *_s_r, __attribute__((unused)) ptrdiff_t nbytes) {
  return (void*)0;
}
