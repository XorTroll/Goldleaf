#ifndef HACTOOL_PACKAGES_H
#define HACTOOL_PACKAGES_H

#include "types.h"
#include "utils.h"
#include "settings.h"
#include "kip.h"

#define MAGIC_PK11 0x31314B50
#define MAGIC_PK21 0x31324B50

typedef struct {
    unsigned char build_hash[0x10];
    unsigned char build_date[0x10];
    unsigned char stage1[0x3FC0];
    uint32_t pk11_size;
    unsigned char _0x3FE4[0xC];
    unsigned char ctr[0x10];
} pk11_stage1_t;

typedef struct {
    uint32_t magic;
    uint32_t warmboot_size;
    uint32_t _0x8;
    uint32_t _0xC;
    uint32_t nx_bootloader_size;
    uint32_t _0x14;
    uint32_t secmon_size;
    uint32_t _0x1C;
    unsigned char data[];
} pk11_t;

typedef struct {
    FILE *file;
    hactool_ctx_t *tool_ctx;
    int is_pilot;
    unsigned int key_rev;
    pk11_stage1_t stage1;
    pk11_t *pk11;
} pk11_ctx_t;


static inline unsigned char *pk11_get_warmboot_bin(pk11_ctx_t *ctx) {
    return ctx->is_pilot ? &ctx->pk11->data[ctx->pk11->secmon_size + ctx->pk11->nx_bootloader_size] : &ctx->pk11->data[0];
}

static inline unsigned char *pk11_get_secmon(pk11_ctx_t *ctx) {
    return ctx->is_pilot ? &ctx->pk11->data[0] : &ctx->pk11->data[ctx->pk11->warmboot_size + ctx->pk11->nx_bootloader_size];
}

static inline unsigned char *pk11_get_nx_bootloader(pk11_ctx_t *ctx) {
    return ctx->is_pilot ? &ctx->pk11->data[ctx->pk11->secmon_size] : &ctx->pk11->data[ctx->pk11->warmboot_size];
}


void pk11_process(pk11_ctx_t *ctx);
void pk11_print(pk11_ctx_t *ctx);
void pk11_save(pk11_ctx_t *ctx);


/* Package2 */
#pragma pack(push, 1)
typedef struct {
    unsigned char signature[0x100];
    union {
        unsigned char ctr[0x10];
        uint32_t ctr_dwords[0x4];
    };
    unsigned char section_ctrs[4][0x10];
    uint32_t magic;
    uint32_t base_offset;
    uint32_t _0x58;
    uint8_t version_max; /* Must be > TZ value. */
    uint8_t version_min; /* Must be < TZ value. */
    uint16_t _0x5E;
    uint32_t section_sizes[4];
    uint32_t section_offsets[4];
    unsigned char section_hashes[4][0x20];
} pk21_header_t;
#pragma pack(pop)

typedef struct {
    FILE *file;
    hactool_ctx_t *tool_ctx;
    unsigned int key_rev;
    uint32_t package_size;
    validity_t signature_validity;
    validity_t section_validities[4];
    unsigned char *sections;
    pk21_header_t header;
    ini1_ctx_t ini1_ctx;
} pk21_ctx_t;

void pk21_process(pk21_ctx_t *ctx);
void pk21_print(pk21_ctx_t *ctx);
void pk21_save(pk21_ctx_t *ctx);

#endif