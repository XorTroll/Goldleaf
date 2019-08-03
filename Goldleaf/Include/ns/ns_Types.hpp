
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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