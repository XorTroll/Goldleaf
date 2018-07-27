#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <switch.h>
#include <switch/services/ncm.h>

Result parseXML(NcmMetaRecord *metaRecordOut, NcmContentRecord **contentRecordsOut, size_t *numContentRecordsOut, u8 *xmlBuf, size_t xmlBufSize);

#ifdef __cplusplus
}
#endif