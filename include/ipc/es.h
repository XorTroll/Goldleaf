#pragma once

#include <switch.h>

typedef struct {
    u8 c[0x10];
} RightsId;

Result esInitialize();
void esExit();

Result esImportTicket(void const *tikBuf, size_t tikSize, void const *certBuf, size_t certSize);
Result esGetTitleKey(const RightsId *rightsId, u8 *outBuf, size_t bufSize);