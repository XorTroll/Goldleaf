
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <ns/ns_Types.hpp>

namespace ns
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();
    
    Result DeleteApplicationCompletely(u64 ApplicationId);
    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize);
    Result ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, void *Out, size_t OutBufferSize, u32 *out_Count);
    Result DeleteApplicationRecord(u64 ApplicationId);
    Result CountApplicationContentMeta(u64 ApplicationId, u32 *out_Count);
    Result PushLaunchVersion(u64 ApplicationId, u64 LaunchVersion);
}