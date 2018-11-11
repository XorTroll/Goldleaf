
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>

namespace gleaf::ns
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