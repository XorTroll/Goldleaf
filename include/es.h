#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

Result esInitialize();
void esExit();

Result esGetTitleKey(u64 titleId, u8 *outBuf, size_t bufSize);

#ifdef __cplusplus
}
#endif