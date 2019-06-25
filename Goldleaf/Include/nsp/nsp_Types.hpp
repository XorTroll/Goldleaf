
#pragma once
#include <switch.h>

namespace nsp
{
    struct PFS0Header
    {
        u32 Magic;
        u32 FileCount;
        u32 StringTableSize;
        u32 Reserved;
    } PACKED;

    struct PFS0FileEntry
    {
        u64 Offset;
        u64 Size;
        u32 StringTableOffset;
        u32 Pad;
    } PACKED;

    struct PFS0File
    {
        PFS0FileEntry Entry;
        std::string Name;
    };

    static constexpr u32 Magic = 0x30534650;
}