// Copied from SwitchIdent (https://github.com/joel16/SwitchIdent)

#pragma once

#include <switch.h>

typedef struct {
    u8 version_raw[8];
    char platform[32];
    char hash[64];
    char version_short[24];
    char version_long[128];
} SetSysFirmwareVersion;

Result setsysGetFirmwareVersion(Service *srv, SetSysFirmwareVersion *ver);
