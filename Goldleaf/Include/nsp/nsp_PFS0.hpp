
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <fs/fs_Explorer.hpp>
#include <nsp/nsp_Types.hpp>

namespace nsp
{
    class PFS0
    {
        public:
            PFS0(fs::Explorer *Exp, pu::String Path);
            ~PFS0();
            u32 GetCount();
            pu::String GetFile(u32 Index);
            u64 ReadFromFile(u32 Index, u64 Offset, u64 Size, u8 *Out);
            std::vector<pu::String> GetFiles();
            bool IsOk();
            fs::Explorer *GetExplorer();
            u64 GetFileSize(u32 Index);
            void SaveFile(u32 Index, fs::Explorer *Exp, pu::String Path);
            u32 GetFileIndexByName(pu::String File);
        private:
            pu::String path;
            fs::Explorer *gexp;
            u8 *stringtable;
            u32 headersize;
            PFS0Header header;
            std::vector<PFS0File> files;
            bool ok;
    };
}
