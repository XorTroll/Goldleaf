/**
 * @file ncm.h
 * @brief Content Manager (ncm) service IPC wrapper.
 * @copyright libnx Authors
 */
#pragma once
#include <switch.h>
#include "es.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum NCMContentType {
    NCMContentType_CNMT = 0,
    NCMContentType_Program = 1,
    NCMContentType_Data = 2,
    NCMContentType_Icon = 3,
    NCMContentType_Doc = 4,
    NCMContentType_Info = 5,
} NCMContentType;

typedef struct {
    Service s;
} NCMContentStorage;

typedef struct {
    Service s;
} NCMContentMetaDatabase;

typedef struct {
    u64 titleID;    // 0x0
    u32 version;    // 0x8
    u8  type;       // 0xc
    u8  flags;      // 0xd
    u8  padding[2]; // 0xe
} NCMMetaRecord;

typedef struct {
    u8 c[0x10];
} NCMNCAID;

typedef struct {
    NCMNCAID  ncaID;
    u8        size[0x6];
    u8        type;
    u8        padding;
} NCMContentRecord;

typedef struct
{
    NCMMetaRecord metaRecord;
    u64           baseTitleID;
} NCMApplicationContentMetaKey;

Result ncmInitialize();
void ncmExit();

Result ncmOpenContentStorage(FsStorageId storage, NCMContentStorage* out);
Result ncmContentStorageGetPath(NCMContentStorage* cs, const NCMNCAID* ncaId, char* out);
Result ncmContentStorageGetSize(NCMContentStorage* cs, const NCMNCAID* ncaId, u64* out);
Result ncmContentStorageReadContentIdFile(NCMContentStorage* cs, const NCMNCAID* ncaId, u64 offset, char* outBuf, size_t bufSize);
Result ncmContentStorageHas(NCMContentStorage* cs, const NCMNCAID *ncaId, bool *out);
Result ncmContentStorageGetRightsIdFromContentId(NCMContentStorage* cs, const NCMNCAID *ncaId, RightsId *rightsIdOut, u64 *unkOut);

Result ncmOpenContentMetaDatabase(FsStorageId storage, NCMContentMetaDatabase* out);
Result ncmContentMetaDatabaseSet(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u64 content_record_size, NCMContentRecord *content_records_in);
Result ncmContentMetaDatabaseGet(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u64 content_record_size, NCMContentRecord *content_records_out, u64 *size_read_out);
Result ncmContentMetaDatabaseRemove(NCMContentMetaDatabase* db, const NCMMetaRecord *record);
Result ncmContentMetaDatabaseListContentInfo(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u32 index, NCMContentRecord *content_records_out, size_t out_buf_size, u32 *entries_read_out);
Result ncmContentMetaDatabaseGetLatestContentMetaKey(NCMContentMetaDatabase* db, u64 titleID, NCMMetaRecord* out);
Result ncmContentMetaDatabaseGetContentIdByType(NCMContentMetaDatabase* db, NCMContentType contentType, const NCMMetaRecord* record, NCMNCAID* out);
Result ncmContentMetaDatabaseListApplication(NCMContentMetaDatabase* db, u8 filter, NCMApplicationContentMetaKey *entries_buf_out, size_t entries_buf_size, u32 *num_entries_total_out, u32 *num_entries_written_out);
Result ncmContentMetaDatabaseHas(NCMContentMetaDatabase* db, const NCMMetaRecord *record, bool *out);
Result ncmContentMetaDatabaseCommit(NCMContentMetaDatabase* db);

#ifdef __cplusplus
}
#endif