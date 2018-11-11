#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <switch/types.h>

extern FILE *nxlinkout;

int nxLinkInitialize(void);
void nxLinkExit(void);

void printBytes(FILE* out, u8 *bytes, size_t size, bool includeHeader);

#ifdef __cplusplus
}
#endif