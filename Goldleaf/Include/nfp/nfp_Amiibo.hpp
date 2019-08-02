
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
    Result Initialize();
    bool IsReady();
    Result Open();
    NfpuTagInfo GetTagInfo();
    NfpuRegisterInfo GetRegisterInfo();
    NfpuCommonInfo GetCommonInfo();
    NfpuModelInfo GetModelInfo();
    Result DumpToEmuiibo(NfpuTagInfo &tag, NfpuRegisterInfo &reg, NfpuCommonInfo &common, NfpuModelInfo &model);
    void Close();
    void Finalize();
}