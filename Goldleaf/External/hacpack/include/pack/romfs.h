#ifndef HACPACK_ROMFS_H
#define HACPACK_ROMFS_H

#include <sys/types.h>
#include "filepath.h"
#include "ivfc.h"

typedef struct romfs_dirent_ctx {
    filepath_t sum_path;
    filepath_t cur_path;
    uint32_t entry_offset;
    struct romfs_dirent_ctx *parent; /* Parent node */
    struct romfs_dirent_ctx *child; /* Child node */
    struct romfs_dirent_ctx *sibling; /* Sibling node */
    struct romfs_fent_ctx *file; /* File node */
    struct romfs_dirent_ctx *next; /* Next node */
} romfs_dirent_ctx_t;

typedef struct romfs_fent_ctx {
    filepath_t sum_path;
    filepath_t cur_path;
    uint32_t entry_offset;
    uint64_t offset;
    uint64_t size;
    romfs_dirent_ctx_t *parent; /* Parent dir */
    struct romfs_fent_ctx *sibling; /* Sibling file */
    struct romfs_fent_ctx *next; /* Logical next file */
} romfs_fent_ctx_t;

typedef struct {
    romfs_fent_ctx_t *files;
    uint64_t num_dirs;
    uint64_t num_files;
    uint64_t dir_table_size;
    uint64_t file_table_size;
    uint64_t dir_hash_table_size;
    uint64_t file_hash_table_size;
    uint64_t file_partition_size;
} romfs_ctx_t;

#pragma pack(push, 1)
typedef struct {
    uint64_t header_size;
    uint64_t dir_hash_table_ofs;
    uint64_t dir_hash_table_size;
    uint64_t dir_table_ofs;
    uint64_t dir_table_size;
    uint64_t file_hash_table_ofs;
    uint64_t file_hash_table_size;
    uint64_t file_table_ofs;
    uint64_t file_table_size;
    uint64_t file_partition_ofs;
} romfs_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t parent;
    uint32_t sibling;
    uint32_t child;
    uint32_t file;
    uint32_t hash;
    uint32_t name_size;
    char name[];
} romfs_direntry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t parent;
    uint32_t sibling;
    uint64_t offset;
    uint64_t size;
    uint32_t hash;
    uint32_t name_size;
    char name[];
} romfs_fentry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    ivfc_hdr_t ivfc_header;
    uint8_t _0xE0[0x58];
} romfs_superblock_t;
#pragma pack(pop)

size_t romfs_build(filepath_t *in_dirpath, filepath_t *out_romfspath, uint64_t *out_size);

#endif