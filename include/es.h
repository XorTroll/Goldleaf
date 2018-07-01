#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u8 c[0x10];
} RightsId;

Result esInitialize();
void esExit();

Result esGetTitleKey(const RightsId *rightsId, u8 *outBuf, size_t bufSize);

#ifdef __cplusplus
}
#endif