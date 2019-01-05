#ifndef HACPACK_SETTINGS_H
#define HACPACK_SETTINGS_H

#include <stdio.h>
#include <stdint.h>
#include "types.h"
#include "filepath.h"

typedef struct
{
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
} hp_keyset_t;

enum hp_nca_type
{
    NCA_TYPE_PROGRAM = 0,
    NCA_TYPE_META = 1,
    NCA_TYPE_CONTROL = 2,
    NCA_TYPE_MANUAL = 3,
    NCA_TYPE_DATA = 4,
    NCA_TYPE_PUBLICDATA = 5
};

enum hp_file_type
{
    FILE_TYPE_NCA,
    FILE_TYPE_NSP
};

enum hp_title_type
{
    TITLE_TYPE_SYSTEMPROGRAM = 0x1,
    TITLE_TYPE_SYSTEMDATA = 0x2,
    TITLE_TYPE_APPLICATION = 0x80,
    TITLE_TYPE_PATCH = 0x81,
    TITLE_TYPE_ADDON = 0x82
};

enum nca_sig_type
{
    NCA_SIG_TYPE_ZERO = 0,
    NCA_SIG_TYPE_STATIC = 1,
    NCA_SIG_TYPE_RANDOM = 2
};

enum nca_distribution_type
{
    NCA_DISTRIBUTION_DOWNLOAD = 0,
    NCA_DISTRIBUTION_GAMECARD = 1
};

typedef struct
{
    hp_keyset_t keyset;
    uint64_t title_id;
    filepath_t temp_dir;
    filepath_t out_dir;
    filepath_t exefs_dir;
    filepath_t romfs_dir;
    filepath_t logo_dir;
    filepath_t programnca;
    filepath_t controlnca;
    filepath_t legalnca;
    filepath_t htmldocnca;
    filepath_t metanca;
    filepath_t datanca;
    filepath_t publicdatanca;
    filepath_t ncadir;
    filepath_t cnmt;
    filepath_t backup_dir;
    uint8_t plaintext;
    uint8_t digest[0x20];
    uint32_t title_version;
    uint8_t has_title_key;
    uint8_t nozeroacidsig;
    uint8_t nozeroacidkey;
    unsigned char title_key[0x10];
    unsigned char *keyareakey;
    int keygeneration;
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
    enum hp_nca_type nca_type;
    enum hp_file_type file_type;
    enum hp_title_type title_type;
    enum nca_sig_type nca_sig;
    enum nca_distribution_type nca_disttype;
} hp_settings_t;

#endif
