#ifndef HACPACK_NCA_H
#define HACPACK_NCA_H

#include "types.h"
#include "settings.h"
#include "aes.h"
#include "pfs0.h"
#include "romfs.h"
#include "ivfc.h"

#define MAGIC_NCA3 0x3341434E /* "NCA3" */

#pragma pack(push, 1)
typedef struct
{
    uint32_t media_start_offset;
    uint32_t media_end_offset;
    uint8_t _0x8[0x8]; /* Padding. */
} nca_section_entry_t;
#pragma pack(pop)

typedef enum
{
    FS_TYPE_ROMFS = 0,
    FS_TYPE_PFS0 = 1
} section_fs_type_t;

typedef enum
{
    HASH_TYPE_PFS0 = 2,
    HASH_TYPE_ROMFS = 3
} section_hash_type_t;

typedef enum
{
    CRYPT_NONE = 1,
    CRYPT_XTS = 2,
    CRYPT_CTR = 3,
    CRYPT_BKTR = 4,
} section_crypt_type_t;

/* NCA FS header. */
#pragma pack(push, 1)
typedef struct
{
    uint16_t version;
    uint8_t fs_type;
    uint8_t hash_type;
    uint8_t crypt_type;
    uint8_t _0x5[0x3];
    union { /* FS-specific superblock. Size = 0x138. */
        pfs0_superblock_t pfs0_superblock;
        romfs_superblock_t romfs_superblock;
    };
    uint8_t section_ctr[0x8];
    uint8_t _0x148[0xB8]; /* Padding. */
} nca_fs_header_t;
#pragma pack(pop)

/* Nintendo content archive header. */
#pragma pack(push, 1)
typedef struct
{
    uint8_t fixed_key_sig[0x100]; /* RSA-PSS signature over header with fixed key. */
    uint8_t npdm_key_sig[0x100];  /* RSA-PSS signature over header with key in NPDM. */
    uint32_t magic;
    uint8_t distribution; /* System vs gamecard. */
    uint8_t content_type;
    uint8_t crypto_type; /* Which keyblob (field 1) */
    uint8_t kaek_ind;    /* Which kaek index? */
    uint64_t nca_size;   /* Entire archive size. */
    uint64_t title_id;
    uint8_t _0x218[0x4]; /* Padding. */
    union {
        uint32_t sdk_version; /* What SDK was this built with? */
        struct
        {
            uint8_t sdk_revision;
            uint8_t sdk_micro;
            uint8_t sdk_minor;
            uint8_t sdk_major;
        };
    };
    uint8_t crypto_type2;                   /* Which keyblob (field 2) */
    uint8_t _0x221[0xF];                    /* Padding. */
    uint8_t rights_id[0x10];                /* Rights ID (for titlekey crypto). */
    nca_section_entry_t section_entries[4]; /* Section entry metadata. */
    uint8_t section_hashes[4][0x20];        /* SHA-256 hashes for each section header. */
    uint8_t encrypted_keys[4][0x10];        /* Encrypted key area. */
    uint8_t _0x340[0xC0];                   /* Padding. */
    nca_fs_header_t fs_headers[4];          /* FS section headers. */
} nca_header_t;
#pragma pack(pop)

void nca_create_romfs_type(hp_settings_t *settings, char *nca_type);
void nca_create_program(hp_settings_t *settings);
void nca_create_meta(hp_settings_t *settings);
void nca_write_padding(FILE *nca_file);
void nca_write_file(FILE *nca_file, filepath_t *ivfc_level_path);
void nca_calculate_section_hash(nca_fs_header_t *fs_header, uint8_t *out_section_hash);
void nca_calculate_hash(FILE *nca_file, unsigned char *out_nca_hash);
void nca_encrypt_key_area(nca_header_t *nca_header, hp_settings_t *settings);
void nca_encrypt_header(nca_header_t *nca_header, hp_settings_t *settings);
void nca_encrypt_section(FILE *nca_file, nca_header_t *nca_header, uint8_t section_index, hp_settings_t *settings);
void nca_update_ctr(unsigned char *ctr, uint64_t ofs);
void nca_set_keygen(nca_header_t *nca_header, hp_settings_t *settings);
void nca_generate_sig(uint8_t *nca_sig, hp_settings_t *settings);
char *nca_romfs_get_type(uint8_t type);

#endif
