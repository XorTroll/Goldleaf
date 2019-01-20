
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <switch/arm/atomics.h>
#include <cstring>
#include <tuple>
#include <gleaf/ns/Record.hpp>

namespace gleaf::ns
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();
    
    Result DeleteApplicationCompletely(u64 ApplicationId);
    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize);
    std::tuple<Result, u32, void*> ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, size_t OutBufferSize);
    Result DeleteApplicationRecord(u64 ApplicationId);
    std::tuple<Result, u32> CountApplicationContentMeta(u64 ApplicationId);
}