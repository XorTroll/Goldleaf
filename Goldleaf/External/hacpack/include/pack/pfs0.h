#ifndef HACPACK_PFS0_H
#define HACPACK_PFS0_H

#include <stdint.h>
#include "types.h"
#include "utils.h"
#include "filepath.h"

#define MAGIC_PFS0 0x30534650
#define PFS0_EXEFS_HASH_BLOCK_SIZE 0x10000;
#define PFS0_LOGO_HASH_BLOCK_SIZE 0x1000;
#define PFS0_META_HASH_BLOCK_SIZE 0x1000;
#define PFS0_PADDING_SIZE 0x200;

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint32_t num_files;
    uint32_t string_table_size;
    uint32_t reserved;
} pfs0_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint64_t offset;
    uint64_t size;
    uint32_t string_table_offset;
    uint32_t reserved;
} pfs0_file_entry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t master_hash[0x20]; /* SHA-256 hash of the hash table. */
    uint32_t block_size; /* In bytes. */
    uint32_t always_2;
    uint64_t hash_table_offset; /* Normally zero. */
    uint64_t hash_table_size; 
    uint64_t pfs0_offset;
    uint64_t pfs0_size;
    uint8_t _0x48[0xF0];
} pfs0_superblock_t;
#pragma pack(pop)

int pfs0_build(filepath_t *in_dirpath, filepath_t *out_pfs0_filepath, uint64_t *out_pfs0_size);
void pfs0_create_hashtable(filepath_t *pfs0_path, filepath_t *pfs0_hashtable_path, uint32_t hash_block_size, uint64_t *out_hashtable_size, uint64_t *out_pfs0_offset);
void pfs0_calculate_master_hash(filepath_t *pfs0_hashtable_filepath, uint64_t hash_table_size, uint8_t *out_master_hash);

#endif
