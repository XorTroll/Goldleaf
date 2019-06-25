
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>

namespace ns
{
    struct ApplicationRecord
    {
        u64 ApplicationId;
        u64 Unknown;
        u64 Size;
    } PACKED;

    struct ContentStorageRecord
    {
        NcmMetaRecord Record;
        u64 StorageId;
    } PACKED;
}