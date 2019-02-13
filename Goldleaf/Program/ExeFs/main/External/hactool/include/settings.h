#ifndef HACTOOL_SETTINGS_H
#define HACTOOL_SETTINGS_H
#include <stdio.h>
#include "types.h"
#include "filepath.h"

typedef enum {
    KEYSET_DEV,
    KEYSET_RETAIL
} keyset_variant_t;

typedef enum {
    BASEFILE_ROMFS,
    BASEFILE_NCA,
    BASEFILE_FAKE
} hactool_basefile_t;

typedef struct {
    unsigned char secure_boot_key[0x10];                 /* Secure boot key for use in key derivation. NOTE: CONSOLE UNIQUE. */
    unsigned char tsec_key[0x10];                        /* TSEC key for use in key derivation. NOTE: CONSOLE UNIQUE. */
    unsigned char keyblob_keys[0x20][0x10];              /* Actual keys used to decrypt keyblobs. NOTE: CONSOLE UNIQUE.*/
    unsigned char keyblob_mac_keys[0x20][0x10];          /* Keys used to validate keyblobs. NOTE: CONSOLE UNIQUE. */ 
    unsigned char encrypted_keyblobs[0x20][0xB0];        /* Actual encrypted keyblobs (EKS). NOTE: CONSOLE UNIQUE. */ 
    unsigned char keyblobs[0x20][0x90];                  /* Actual decrypted keyblobs (EKS). */ 
    unsigned char keyblob_key_sources[0x20][0x10];       /* Seeds for keyblob keys. */
    unsigned char keyblob_mac_key_source[0x10];          /* Seed for keyblob MAC key derivation. */
    unsigned char tsec_root_key[0x10];                   /* Seed for master kek decryption, from TSEC firmware on 6.2.0+. */
    unsigned char master_kek_sources[0x20][0x10];        /* Seeds for firmware master keks. */
    unsigned char master_keks[0x20][0x10];               /* Firmware master keks, stored in keyblob prior to 6.2.0. */
    unsigned char master_key_source[0x10];               /* Seed for master key derivation. */
    unsigned char master_keys[0x20][0x10];               /* Firmware master keys. */
    unsigned char package1_keys[0x20][0x10];             /* Package1 keys. */
    unsigned char package2_keys[0x20][0x10];             /* Package2 keys. */
    unsigned char package2_key_source[0x10];             /* Seed for Package2 key. */
    unsigned char aes_kek_generation_source[0x10];       /* Seed for GenerateAesKek, usecase + generation 0. */
    unsigned char aes_key_generation_source[0x10];       /* Seed for GenerateAesKey. */
    unsigned char key_area_key_application_source[0x10]; /* Seed for kaek 0. */
    unsigned char key_area_key_ocean_source[0x10];       /* Seed for kaek 1. */
    unsigned char key_area_key_system_source[0x10];      /* Seed for kaek 2. */
    unsigned char titlekek_source[0x10];                 /* Seed for titlekeks. */
    unsigned char header_kek_source[0x10];               /* Seed for header kek. */
    unsigned char sd_card_kek_source[0x10];              /* Seed for SD card kek. */
    unsigned char sd_card_key_sources[2][0x20];          /* Seed for SD card encryption keys. */
    unsigned char save_mac_kek_source[0x10];             /* Seed for save kek. */
    unsigned char save_mac_key_source[0x10];             /* Seed for save key. */
    unsigned char header_key_source[0x20];               /* Seed for NCA header key. */
    unsigned char header_key[0x20];                      /* NCA header key. */
    unsigned char titlekeks[0x20][0x10];                 /* Title key encryption keys. */
    unsigned char key_area_keys[0x20][3][0x10];          /* Key area encryption keys. */
    unsigned char sd_card_keys[2][0x20];
    unsigned char nca_hdr_fixed_key_modulus[0x100];      /* NCA header fixed key RSA pubk. */
    unsigned char acid_fixed_key_modulus[0x100];         /* ACID fixed key RSA pubk. */
    unsigned char package2_fixed_key_modulus[0x100];     /* Package2 Header RSA pubk. */
} nca_keyset_t;

typedef struct {
    int enabled;
    filepath_t path;
} override_filepath_t;

typedef struct {
    unsigned char rights_id[0x10];
    unsigned char titlekey[0x10];
    unsigned char dec_titlekey[0x10];
} titlekey_entry_t;

typedef struct {
    unsigned int count;
    titlekey_entry_t *titlekeys;
} known_titlekeys_t;

typedef struct {
    nca_keyset_t keyset;
    int has_cli_titlekey;
    unsigned char cli_titlekey[0x10];
    unsigned char dec_cli_titlekey[0x10];
    known_titlekeys_t known_titlekeys;
    int has_cli_contentkey;
    unsigned char cli_contentkey[0x10];
    int has_sdseed;
    unsigned char sdseed[0x10];
    unsigned char keygen_sbk[0x10];
    unsigned char keygen_tsec[0x10];
    filepath_t section_paths[4];
    filepath_t section_dir_paths[4];
    override_filepath_t exefs_path;
    override_filepath_t exefs_dir_path;
    override_filepath_t romfs_path;
    override_filepath_t romfs_dir_path;
    override_filepath_t out_dir_path;
    filepath_t pfs0_dir_path;
    filepath_t hfs0_dir_path;
    filepath_t pk11_dir_path;
    filepath_t pk21_dir_path;
    filepath_t ini1_dir_path;
    filepath_t plaintext_path;
    filepath_t uncompressed_path;
    filepath_t rootpt_dir_path;
    filepath_t update_dir_path;
    filepath_t normal_dir_path;
    filepath_t secure_dir_path;
    filepath_t logo_dir_path;
    filepath_t header_path;
    filepath_t nax0_path;
    filepath_t nax0_sd_path;
    filepath_t npdm_json_path;
} hactool_settings_t;

enum hactool_file_type
{
    FILETYPE_NCA,
    FILETYPE_PFS0,
    FILETYPE_ROMFS,
    FILETYPE_NCA0_ROMFS,
    FILETYPE_HFS0,
    FILETYPE_XCI,
    FILETYPE_NPDM,
    FILETYPE_PACKAGE1,
    FILETYPE_PACKAGE2,
    FILETYPE_INI1,
    FILETYPE_KIP1,
    FILETYPE_NSO0,
    FILETYPE_NAX0,
    FILETYPE_BOOT0
};

#define ACTION_INFO (1<<0)
#define ACTION_EXTRACT (1<<1)
#define ACTION_VERIFY (1<<2)
#define ACTION_RAW (1<<3)
#define ACTION_LISTROMFS (1<<4)
#define ACTION_DEV (1<<5)
#define ACTION_EXTRACTINI1 (1<<6)
#define ACTION_ONLYUPDATEDROMFS (1<<7)
#define ACTION_SAVEINIJSON (1<<8)

struct nca_ctx; /* This will get re-defined by nca.h. */

typedef struct {
    enum hactool_file_type file_type;
    FILE *file;
    FILE *base_file;
    hactool_basefile_t base_file_type;
    struct nca_ctx *base_nca_ctx;
    hactool_settings_t settings;
    uint32_t action;
} hactool_ctx_t;

#endif
