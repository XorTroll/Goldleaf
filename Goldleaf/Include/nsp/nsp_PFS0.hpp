#pragma once
#include <fs/fs_Explorer.hpp>
#include <nsp/nsp_Types.hpp>

namespace nsp
{
    class PFS0
    {
        public:
            PFS0(fs::Explorer *Exp, std::string Path);
            ~PFS0();
            u32 GetCount();
            std::string GetFile(u32 Index);
            u64 ReadFromFile(u32 Index, u64 Offset, u64 Size, u8 *Out);
            std::vector<std::string> GetFiles();
            bool IsOk();
            fs::Explorer *GetExplorer();
            u64 GetFileSize(u32 Index);
            void SaveFile(u32 Index, fs::Explorer *Exp, std::string Path);
            u32 GetFileIndexByName(std::string File);
        private:
            std::string path;
            fs::Explorer *gexp;
            u8 *stringtable;
            u32 headersize;
            PFS0Header header;
            std::vector<PFS0File> files;
            bool ok;
    };
}
