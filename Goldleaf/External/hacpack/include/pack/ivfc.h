#ifndef HACPACK_IVFC_H
#define HACPACK_IVFC_H

#include "types.h"
#include "utils.h"
#include "filepath.h"


#define IVFC_HEADER_SIZE 0xE0
#define IVFC_MAX_LEVEL 6
#define IVFC_MAX_BUFFERSIZE 0x4000
#define IVFC_HASH_BLOCK_SIZE 0x4000;

#define MAGIC_IVFC 0x43465649

#pragma pack(push, 1)
typedef struct {
    uint64_t logical_offset;
    uint64_t hash_data_size;
    uint32_t block_size;
    uint32_t reserved;
} ivfc_level_hdr_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    uint32_t id;
    uint32_t master_hash_size;
    uint32_t num_levels;
    ivfc_level_hdr_t level_headers[IVFC_MAX_LEVEL];
    uint8_t _0xA0[0x20];
    uint8_t master_hash[0x20];
} ivfc_hdr_t;
#pragma pack(pop)

void ivfc_create_level(filepath_t *dst_level_file, filepath_t *src_level_file, uint64_t *out_size);
void ivfc_calculate_master_hash(filepath_t *ivfc_level1_filepath ,uint8_t *out_master_hash);

#endif
