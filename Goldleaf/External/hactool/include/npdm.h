#ifndef HACTOOL_NPDM_H
#define HACTOOL_NPDM_H

#include "types.h"
#include "utils.h"
#include "settings.h"
#include "cJSON.h"

#define MAGIC_META 0x4154454D
#define MAGIC_ACID 0x44494341
#define MAGIC_ACI0 0x30494341

typedef struct kac_mmio {
    uint64_t address;
    uint64_t size;
    int is_ro;
    int is_norm;
    struct kac_mmio *next;
} kac_mmio_t;

typedef struct kac_irq {
    uint32_t irq0;
    uint32_t irq1;
    struct kac_irq *next;
} kac_irq_t;

typedef struct {
    int has_kern_flags;
    uint32_t lowest_thread_prio;
    uint32_t highest_thread_prio;
    uint32_t lowest_cpu_id;
    uint32_t highest_cpu_id;
    uint8_t svcs_allowed[0x80];
    kac_mmio_t *mmio;
    kac_irq_t *irqs;
    int has_app_type;
    uint32_t application_type;
    int has_kern_ver;
    uint32_t kernel_release_version;
    int has_handle_table_size;
    uint32_t handle_table_size;
    int has_debug_flags;
    int allow_debug;
    int force_debug;
} kac_t;

typedef struct sac_entry {
    char service[0x11];
    int valid;
    struct sac_entry *next;
} sac_entry_t;

/* FAC, FAH need to be tightly packed. */
#pragma pack(push, 1)
typedef struct {
    uint32_t version;
    uint64_t perms;
    uint8_t _0xC[0x20];
} fac_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t version;
    uint64_t perms;
    uint32_t _0xC;
    uint32_t _0x10;
    uint32_t _0x14;
    uint32_t _0x18;
} fah_t;
#pragma pack(pop)

typedef struct {
    const char *name;
    uint64_t mask;
} fs_perm_t;

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

static inline npdm_acid_t *npdm_get_acid(npdm_t *npdm) {
    return (npdm_acid_t *)((char *)npdm + npdm->acid_offset);
}

static inline npdm_aci0_t *npdm_get_aci0(npdm_t *npdm) {
    return (npdm_aci0_t *)((char *)npdm + npdm->aci0_offset);
}

void npdm_process(npdm_t *npdm, hactool_ctx_t *tool_ctx);
void npdm_print(npdm_t *npdm, hactool_ctx_t *tool_ctx);
void npdm_save(npdm_t *npdm, hactool_ctx_t *tool_ctx);

const char *npdm_get_proc_category(int process_category);
void kac_print(const uint32_t *descriptors, uint32_t num_descriptors);
char *npdm_get_json(npdm_t *npdm);

void cJSON_AddU8ToObject(cJSON *obj, const char *name, uint8_t val);
void cJSON_AddU16ToObject(cJSON *obj, const char *name, uint16_t val);
void cJSON_AddU32ToObject(cJSON *obj, const char *name, uint32_t val);
void cJSON_AddU64ToObject(cJSON *obj, const char *name, uint64_t val);
cJSON *kac_get_json(const uint32_t *descriptors, uint32_t num_descriptors);


#endif
