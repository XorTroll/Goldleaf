#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <switch.h>

Result getTitleName(u64 titleId, char *outBuf, size_t bufSize);

#ifdef __cplusplus
}
#endif