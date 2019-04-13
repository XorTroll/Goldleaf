#ifndef HACTOOL_hfs0_H
#define HACTOOL_hfs0_H

#include "types.h"
#include "utils.h"
#include "settings.h"

#define MAGIC_HFS0 0x30534648

typedef struct {
    uint32_t magic;
    uint32_t num_files;
    uint32_t string_table_size;
    uint32_t reserved;
} hfs0_header_t;

typedef struct {
    uint64_t offset;
    uint64_t size;
    uint32_t string_table_offset;
    uint32_t hashed_size;
    uint64_t reserved;
    unsigned char hash[0x20];
} hfs0_file_entry_t;

typedef struct {
    FILE *file;
    uint64_t offset;
    uint64_t size;
    hactool_ctx_t *tool_ctx;
    hfs0_header_t *header;
    const char *name;
} hfs0_ctx_t;

static inline hfs0_file_entry_t *hfs0_get_file_entry(hfs0_header_t *hdr, uint32_t i) {
    if (i >= hdr->num_files) return NULL;
    return (hfs0_file_entry_t *)((char *)(hdr) + sizeof(*hdr) + i * sizeof(hfs0_file_entry_t));
}

static inline char *hfs0_get_string_table(hfs0_header_t *hdr) {
    return (char *)(hdr) + sizeof(*hdr) + hdr->num_files * sizeof(hfs0_file_entry_t);
}

static inline uint64_t hfs0_get_header_size(hfs0_header_t *hdr) {
    return sizeof(*hdr) + hdr->num_files * sizeof(hfs0_file_entry_t) + hdr->string_table_size;
}

static inline char *hfs0_get_file_name(hfs0_header_t *hdr, uint32_t i) {
    return hfs0_get_string_table(hdr) + hfs0_get_file_entry(hdr, i)->string_table_offset;
}

void hfs0_process(hfs0_ctx_t *ctx);
void hfs0_save(hfs0_ctx_t *ctx);
void hfs0_print(hfs0_ctx_t *ctx);

void hfs0_save_file(hfs0_ctx_t *ctx, uint32_t i, filepath_t *dirpath);

#endif