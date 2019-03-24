#ifndef HACTOOL_NCA0_ROMFS_H
#define HACTOOL_NCA0_ROMFS_H

#include "types.h"
#include "utils.h"
#include "ivfc.h"
#include "settings.h"

/* RomFS structs. */
#define NCA0_ROMFS_HEADER_SIZE 0x00000028

typedef struct {
    uint32_t header_size;
    uint32_t dir_hash_table_offset;
    uint32_t dir_hash_table_size;
    uint32_t dir_meta_table_offset;
    uint32_t dir_meta_table_size;
    uint32_t file_hash_table_offset;
    uint32_t file_hash_table_size;
    uint32_t file_meta_table_offset;
    uint32_t file_meta_table_size;
    uint32_t data_offset;
} nca0_romfs_hdr_t;

typedef struct {
    uint8_t master_hash[0x20]; /* SHA-256 hash of the hash table. */
    uint32_t block_size; /* In bytes. */
    uint32_t always_2;
    uint64_t hash_table_offset; /* Normally zero. */
    uint64_t hash_table_size; 
    uint64_t romfs_offset;
    uint64_t romfs_size;
    uint8_t _0x48[0xF0];
} nca0_romfs_superblock_t;

typedef struct {
    nca0_romfs_superblock_t *superblock;
    FILE *file;
    hactool_ctx_t *tool_ctx;
    validity_t superblock_hash_validity;
    validity_t hash_table_validity;
    uint64_t romfs_offset;
    nca0_romfs_hdr_t header;
    romfs_direntry_t *directories;
    romfs_fentry_t *files;
} nca0_romfs_ctx_t;

void nca0_romfs_process(nca0_romfs_ctx_t *ctx);
void nca0_romfs_save(nca0_romfs_ctx_t *ctx);
void nca0_romfs_print(nca0_romfs_ctx_t *ctx);


#endif
