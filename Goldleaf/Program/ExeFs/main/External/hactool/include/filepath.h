#ifndef HACTOOL_FILEPATH_H
#define HACTOOL_FILEPATH_H

#include "types.h"
#include "utils.h"

typedef uint16_t utf16char_t;

#ifdef _WIN32
typedef wchar_t oschar_t; /* utf-16 */

#define os_fopen _wfopen
#define OS_MODE_READ L"rb"
#define OS_MODE_WRITE L"wb"
#define OS_MODE_EDIT L"rb+"
#define OS_PATH_SEPARATOR "\\"
#else
typedef char oschar_t; /* utf-8 */

#define os_fopen fopen
#define OS_MODE_READ "rb"
#define OS_MODE_WRITE "wb"
#define OS_MODE_EDIT "rb+"
#define OS_PATH_SEPARATOR "/"
#endif

typedef struct filepath {
    char char_path[MAX_PATH];
    oschar_t os_path[MAX_PATH];
    validity_t valid;
} filepath_t;

void os_strcpy(oschar_t *dst, const char *src);
int os_makedir(const oschar_t *dir);
int os_rmdir(const oschar_t *dir);

void filepath_init(filepath_t *fpath);
void filepath_copy(filepath_t *fpath, filepath_t *copy);
void filepath_append(filepath_t *fpath, const char *format, ...);
void filepath_append_n(filepath_t *fpath, uint32_t n, const char *format, ...);
void filepath_set(filepath_t *fpath, const char *path);
oschar_t *filepath_get(filepath_t *fpath);



#endif
