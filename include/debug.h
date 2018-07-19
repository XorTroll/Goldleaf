#pragma once

#include <stdio.h>

#include <switch.h>

extern FILE *nxlinkout;

int nxLinkInitialize(void);
void nxLinkExit(void);

void printBytes(u8 *bytes, size_t size);