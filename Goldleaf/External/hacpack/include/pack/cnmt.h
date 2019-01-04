#ifndef HACPACK_CNMT_H
#define HACPACK_CNMT_H

#include <stdint.h>
#include "settings.h"

#pragma pack(push, 1)
typedef struct {
    uint64_t title_id;
    uint32_t title_version;
    uint8_t type;
    uint8_t _0xD;
    uint16_t extended_header_size;
    uint16_t content_entry_count;
    uint16_t meta_entry_count;
    uint8_t _0x14[0xC];
} cnmt_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint64_t patch_title_id;
    uint32_t required_system_version;
    uint32_t padding;
} cnmt_extended_application_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint64_t application_title_id;
    uint32_t required_application_version;
    uint32_t padding;
} cnmt_extended_addon_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    unsigned char hash[0x20];
    unsigned char ncaid[0x10];
    uint8_t size[0x06];
    uint8_t type;
    uint8_t _0x37;
} cnmt_content_record_t;
#pragma pack(pop)

typedef struct {
    cnmt_header_t header;
    uint8_t content_records_count;
    cnmt_content_record_t content_records[5];
} cnmt_ctx_t;

void cnmt_create_application(filepath_t *cnmt_filepath, hp_settings_t *settings);
void cnmt_create_addon(filepath_t *cnmt_filepath, hp_settings_t *settings);
void cnmt_create_systemprogram(filepath_t *cnmt_filepath, hp_settings_t *settings);
void cnmt_create_systemdata(filepath_t *cnmt_filepath, hp_settings_t *settings);
void cnmt_set_content_record(filepath_t *nca_path, cnmt_content_record_t *content_record);

#endif