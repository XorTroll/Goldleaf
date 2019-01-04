#ifndef HACPACK_NPDM_H
#define HACPACK_NPDM_H

#include <stdint.h>
#include "settings.h"

#define MAGIC_META 0x4154454D
#define MAGIC_ACID 0x44494341
#define MAGIC_ACI0 0x30494341

typedef struct {
    uint32_t magic;
    uint32_t _0x4;
    uint32_t _0x8;
    uint8_t mmu_flags;
    uint8_t _0xD;
    uint8_t main_thread_prio;
    uint8_t default_cpuid;
    uint64_t _0x10;
    uint32_t process_category;
    uint32_t main_stack_size;
    char title_name[0x50];
    uint32_t aci0_offset;
    uint32_t aci0_size;
    uint32_t acid_offset;
    uint32_t acid_size;
} npdm_t;

typedef struct {
    uint8_t signature[0x100];
    uint8_t modulus[0x100];
    uint32_t magic;
    uint32_t size;
    uint32_t _0x208;
    uint32_t flags;
    uint64_t title_id_range_min;
    uint64_t title_id_range_max;
    uint32_t fac_offset;
    uint32_t fac_size;
    uint32_t sac_offset;
    uint32_t sac_size;
    uint32_t kac_offset;
    uint32_t kac_size;
    uint64_t padding;
} npdm_acid_t;

typedef struct {
    uint32_t magic;
    uint8_t _0x4[0xC];
    uint64_t title_id;
    uint64_t _0x18;
    uint32_t fah_offset;
    uint32_t fah_size;
    uint32_t sac_offset;
    uint32_t sac_size;
    uint32_t kac_offset;
    uint32_t kac_size;
    uint64_t padding;
} npdm_aci0_t;

void npdm_process(hp_settings_t *settings);

#endif