#define _oscall(rettype,funcname,...) (*((rettype(*)(__VA_ARGS__))(funcname##_##addr)))

#define NULL ((void*)0)
typedef struct{} FILE;
typedef unsigned long size_t;
#define fopen (_oscall(FILE*, fopen, const char * filename, const char * mode))
#define fwrite (_oscall(size_t, fwrite, const void * ptr, size_t size, size_t count, FILE * stream))
#define fclose (_oscall(int, fclose, FILE * stream))
#define malloc (_oscall(void*, malloc, size_t size))
#define free (_oscall(void, free, void * ptr))
#define memset (_oscall(void *, memset, void * ptr, int value, size_t num))
#define mkdir (_oscall(int, mkdir, const char *path, int mode))

#define fopen_addr 0x101850B8
#define fwrite_addr 0x101855A8
#define fclose_addr 0x10184AB0
#define malloc_addr 0x10181AD8
#define free_addr 0x10181ACC
#define memset_addr 0x101860C4
#define mkdir_addr 0x102A8864
