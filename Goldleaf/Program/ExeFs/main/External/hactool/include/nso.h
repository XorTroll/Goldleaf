#ifndef HACTOOL_NSO_H
#define HACTOOL_NSO_H
#include "types.h"
#include "utils.h"
#include "settings.h"

#define MAGIC_NSO0 0x304F534E

typedef struct {
    uint32_t file_off;
    uint32_t dst_off;
    uint32_t decomp_size;
    uint32_t align_or_total_size;
} nso0_segment_t;

typedef struct {
    uint32_t magic;
    uint32_t _0x4;
    uint32_t _0x8;
    uint32_t flags;
    nso0_segment_t segments[3];
    uint8_t build_id[0x20];
    uint32_t compressed_sizes[3];
    uint8_t _0x6C[0x24];
    uint64_t dynstr_extents;
    uint64_t dynsym_extents;
    uint8_t section_hashes[3][0x20];
    unsigned char data[];
} nso0_header_t;

typedef struct {
    FILE *file;
    hactool_ctx_t *tool_ctx;
    nso0_header_t *header;
    nso0_header_t *uncompressed_header;
    validity_t segment_validities[3];
} nso0_ctx_t;

void nso0_process(nso0_ctx_t *ctx);
void nso0_print(nso0_ctx_t *ctx);
void nso0_save(nso0_ctx_t *ctx);

static inline uint64_t nso_get_section_size(nso0_header_t *header, unsigned int segment) {
    int is_compressed = (header->flags >> segment) & 1;
    return is_compressed ? header->compressed_sizes[segment] : header->segments[segment].decomp_size;
}

static inline uint64_t nso_get_size(nso0_header_t *header) {
    return header->segments[2].file_off + nso_get_section_size(header, 2);
}

#endif