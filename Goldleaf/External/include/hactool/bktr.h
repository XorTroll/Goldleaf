#ifndef HACTOOL_BKTR_H
#define HACTOOL_BKTR_H

#include "types.h"

#define MAGIC_BKTR 0x52544B42

typedef struct {
    uint64_t offset;
    uint64_t size;
    uint32_t magic; /* "BKTR" */
    uint32_t _0x14; /* Version? */
    uint32_t num_entries;
    uint32_t _0x1C; /* Reserved? */
} bktr_header_t;

#pragma pack(push, 1)
typedef struct {
    uint64_t virt_offset;
    uint64_t phys_offset;
    uint32_t is_patch;
} bktr_relocation_entry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t _0x0;
    uint32_t num_entries;
    uint64_t virtual_offset_end;
    bktr_relocation_entry_t entries[0x3FF0/sizeof(bktr_relocation_entry_t)];
    uint8_t padding[0x3FF0 % sizeof(bktr_relocation_entry_t)];
} bktr_relocation_bucket_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t _0x0;
    uint32_t num_buckets;
    uint64_t total_size;
    uint64_t bucket_virtual_offsets[0x3FF0/sizeof(uint64_t)];
    bktr_relocation_bucket_t buckets[];
} bktr_relocation_block_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint64_t  offset;
    uint32_t _0x8;
    uint32_t ctr_val;
} bktr_subsection_entry_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t _0x0;
    uint32_t num_entries;
    uint64_t physical_offset_end;
    bktr_subsection_entry_t entries[0x3FF];
} bktr_subsection_bucket_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t _0x0;
    uint32_t num_buckets;
    uint64_t total_size;
    uint64_t bucket_physical_offsets[0x3FF0/sizeof(uint64_t)];
    bktr_subsection_bucket_t buckets[];
} bktr_subsection_block_t;
#pragma pack(pop)

bktr_relocation_bucket_t *bktr_get_relocation_bucket(bktr_relocation_block_t *block, uint32_t i);
bktr_relocation_entry_t *bktr_get_relocation(bktr_relocation_block_t *block, uint64_t offset);

bktr_subsection_bucket_t *bktr_get_subsection_bucket(bktr_subsection_block_t *block, uint32_t i);
bktr_subsection_entry_t *bktr_get_subsection(bktr_subsection_block_t *block, uint64_t offset);

#endif