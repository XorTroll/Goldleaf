#ifndef HACTOOL_UTILS_H
#define HACTOOL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

struct filepath;

#ifdef _WIN32
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

#define FATAL_ERROR(msg) do {\
    fprintf(stderr, "Error: %s\n", msg);\
    exit(EXIT_FAILURE);\
} while (0)

uint32_t align(uint32_t offset, uint32_t alignment);
uint64_t align64(uint64_t offset, uint64_t alignment);

void print_magic(const char *prefix, uint32_t magic);

void memdump(FILE *f, const char *prefix, const void *data, size_t size);

uint64_t _fsize(const char *filename);

void save_file_section(FILE *f_in, uint64_t ofs, uint64_t total_size, struct filepath *filepath);

void save_buffer_to_file(void *buf, uint64_t size, struct filepath *filepath);
void save_buffer_to_directory_file(void *buf, uint64_t size, struct filepath *dirpath, const char *filename);

const char *get_key_revision_summary(uint8_t key_rev);

FILE *open_key_file(const char *prefix);

validity_t check_memory_hash_table(FILE *f_in, unsigned char *hash_table, uint64_t data_ofs, uint64_t data_len, uint64_t block_size, int full_block);
validity_t check_file_hash_table(FILE *f_in, uint64_t hash_ofs, uint64_t data_ofs, uint64_t data_len, uint64_t block_size, int full_block);

#ifdef _MSC_VER
inline int fseeko64(FILE *__stream, long long __off, int __whence)
{
    return _fseeki64(__stream, __off, __whence);
}
#elif __MINGW32__
    /* MINGW32 does not have 64-bit offsets even with large file support. */
    extern int fseeko64 (FILE *__stream, _off64_t __off, int __whence);
#else
    /* off_t is 64-bit with large file support */
    #define fseeko64 fseek
#endif

static inline uint64_t media_to_real(uint64_t media) {
    return MEDIA_SIZE * media;
}

#endif
