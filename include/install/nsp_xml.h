#pragma once

#include <switch.h>
#include <switch/services/ncm.h>

Result parseXML(NcmMetaRecord *metaRecordOut, NcmContentRecord **contentRecordsOut, size_t *numContentRecordsOut, u8 *xmlBuf, size_t xmlBufSize);