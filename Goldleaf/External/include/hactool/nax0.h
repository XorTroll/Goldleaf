#ifndef HACTOOL_NAX0_H
#define HACTOOL_NAX0_H

#include <stdint.h>
#include "types.h"
#include "utils.h"
#include "settings.h"
#include "aes.h"

#define MAGIC_NAX0 0x3058414E

typedef struct {
    uint8_t hmac_header[0x20];
    uint32_t magic;
    uint32_t _0x24;
    uint8_t keys[2][0x10];
    uint64_t size;
    uint8_t _0x50[0x30];
} nax0_header_t;

typedef struct {
    filepath_t base_path;
    hactool_ctx_t *tool_ctx;
    aes_ctx_t *aes_ctx;
    FILE **files;
    unsigned int num_files;
    unsigned int k;
    unsigned char encrypted_keys[2][0x10];
    nax0_header_t header;
} nax0_ctx_t;

void nax0_process(nax0_ctx_t *ctx);
void nax0_save(nax0_ctx_t *ctx);
void nax0_print(nax0_ctx_t *ctx);

#endif