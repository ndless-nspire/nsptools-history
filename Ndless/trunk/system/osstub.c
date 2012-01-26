/* newlib stub that makes newlib happy. When a function is implemented by both newlib and the TI-Nspire OS,
 * the OS version should be used. That's why these functions ar empty. */

#include <os.h>

typedef struct{} _ssize_t;
struct _reent{};
typedef struct{} _off_t;

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

int _getpid_r(void) {
	return 1;
}

int _kill_r(__attribute__((unused)) int pid, __attribute__((unused)) int sig){
	return-1;
}

int _link_r(__attribute__((unused)) char *old, __attribute__((unused)) char *new) {
	return -1;
}

int _times_r(__attribute__((unused)) void *buf){
	return -1;
}

int _wait_r(__attribute__((unused)) int *status) {
	return -1;
}

void _exit(void) {
	exit(-1);
}
