#pragma once

#include <switch/types.h>

namespace tin::install
{
    struct PFS0FileEntry
    {
        u64 dataOffset;
        u64 fileSize;
        u32 stringTableOffset;
        u32 padding;
    } PACKED;

    static_assert(sizeof(PFS0FileEntry) == 0x18, "PFS0FileEntry must be 0x18");

    struct PFS0BaseHeader
    {
        u32 magic;
        u32 numFiles;
        u32 stringTableSize;
        u32 reserved;
    } PACKED;

    static_assert(sizeof(PFS0BaseHeader) == 0x10, "PFS0BaseHeader must be 0x10");
}