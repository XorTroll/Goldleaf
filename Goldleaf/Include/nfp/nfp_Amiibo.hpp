
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <Types.hpp>
#include <fs/fs_Explorer.hpp>

namespace nfp
{
    struct AmiiboData
    {
        u8 uuid[10];
        u8 pad1[0x4a];
        u8 amiibo_id[0x8];
        u8 pad2[0x1c0];
    } PACKED;

    static_assert(sizeof(AmiiboData) == 0x21c, "AmiiboData has an invalid size");

    Result Initialize();
    bool IsReady();
    Result Open();
    NfpuTagInfo GetTagInfo();
    NfpuRegisterInfo GetRegisterInfo();
    NfpuCommonInfo GetCommonInfo();
    NfpuModelInfo GetModelInfo();
    Result DumpToEmuiibo();
    void Close();
    void Finalize();
}