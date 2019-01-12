
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from devkitPro's switch tool's source (build_pfs0)

*/

#pragma once
#include <string>
#include <gleaf/Types.hpp>

namespace gleaf::nsp
{
    struct PFSHeader
    {
        u32 Magic;
        u32 FileCount;
        u32 StrTableSize;
        u32 Reserved;
    };

    struct PFSFileEntry
    {
        u64 Offset;
        u64 Size;
        u32 StrTableOffset;
        u32 Zero;
    };

    int BuildPFS(std::string ContentsDir, std::string OutPFS, std::function<void(u8 Percentage)> Callback);
}