#ifndef HACPACK_FILEPATH_H
#define HACPACK_FILEPATH_H

#include "types.h"
#include <dirent.h>
#include "utils.h"
#ifdef _WIN32
#include <direct.h>
#include <wchar.h>
#include <windows.h>
#endif

#define MAX_SWITCHPATH 0x300

typedef uint16_t utf16char_t;

#ifdef _WIN32
typedef wchar_t oschar_t; /* utf-16 */
typedef _WDIR osdir_t;
typedef struct _wdirent osdirent_t;
typedef struct _stati64 os_stat64_t;

#define os_fopen _wfopen
#define os_opendir _wopendir
#define os_closedir _wclosedir
#define os_readdir _wreaddir
#define os_stat _wstati64
#define os_char_stat _stat64
#define os_fclose fclose
#define os_rename _wrename
#define os_deletefile remove
#define OS_MODE_READ L"rb"
#define OS_MODE_WRITE L"wb"
#define OS_MODE_EDIT L"rb+"
#define OS_MODE_WRITE_EDIT L"wb+"
#define OS_PATH_SEPARATOR "\\"
#else
typedef char oschar_t; /* utf-8 */
typedef DIR osdir_t;
typedef struct dirent osdirent_t;
typedef struct stat os_stat64_t;

#define os_fopen fopen
#define os_opendir opendir
#define os_closedir closedir
#define os_readdir readdir
#define os_stat stat
#define os_char_stat stat
#define os_fclose fclose
#define os_rename rename
#define os_deletefile unlink
#define OS_MODE_READ "rb"
#define OS_MODE_WRITE "wb"
#define OS_MODE_EDIT "rb+"
#define OS_MODE_WRITE_EDIT "wb+"
#define OS_PATH_SEPARATOR "/"
#endif

typedef struct filepath {
    char char_path[MAX_PATH];
    oschar_t os_path[MAX_PATH];
    validity_t valid;
} filepath_t;

void os_strcpy(oschar_t *dst, const char *src);
void os_strncpy_to_char(char *dst, const oschar_t *src, size_t size);
int os_makedir(const oschar_t *dir);
int os_rmdir(const oschar_t *dir);

void filepath_init(filepath_t *fpath);
void filepath_copy(filepath_t *fpath, filepath_t *copy);
void filepath_append(filepath_t *fpath, const char *format, ...);
void filepath_append_n(filepath_t *fpath, uint32_t n, const char *format, ...);
void filepath_os_append(filepath_t *fpath, oschar_t *path);
void filepath_set(filepath_t *fpath, const char *path);
oschar_t *filepath_get(filepath_t *fpath);
int filepath_remove_directory(filepath_t *dir_path);
void filepath_copy_file(filepath_t *source_file, filepath_t *destination_path);

#endif
