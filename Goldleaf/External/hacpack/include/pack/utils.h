#ifndef HACPACK_UTILS_H
#define HACPACK_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#define FATAL_ERROR(msg) do {\
    fprintf(stderr, "Error: %s\n", msg);\
    { p_hexit(1); return; }\
} while (0)


#if defined(_WIN32) || defined(_WIN64)
#define PATH_SEPERATOR '\\'
#else
#define PATH_SEPERATOR '/'
#endif

#define MEDIA_SIZE 0x200

/* On the switch, paths are limited to 0x300. Limit them to 0x400 - 1 on PC. */
/* MAX_PATH is previously defined in "windef.h" on WIN32. */
#ifndef MAX_PATH
#define MAX_PATH 1023
#endif

void memdump(FILE *f, const char *prefix, const void *data, size_t size);

void p_hexit(int rc);
int p_htrc();

#ifdef _MSC_VER
inline int fseeko64(FILE *__stream, long long __off, int __whence)
{
    return _fseeki64(__stream, __off, __whence);
}
inline long long ftello64(FILE * stream)
{
    return _ftelli64(__stream);
}
#elif __MINGW32__
    /* MINGW32 does not have 64-bit offsets even with large file support. */
    extern int fseeko64 (FILE *__stream, _off64_t __off, int __whence);
    extern _off64_t ftello64(FILE * stream);
#else
    /* off_t is 64-bit with large file support */
    #define fseeko64 fseek
	#define ftello64 ftell
#endif

void hexBinaryString(unsigned char *in, int inSize, char *out, int outSize);

#endif
