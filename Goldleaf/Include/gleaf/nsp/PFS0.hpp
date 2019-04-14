#pragma once
#include <gleaf/fs.hpp>

extern "C"
{
    #include <types.h>
    #include <utils.h>
    #include <settings.h>
    #include <pki.h>
    #include <nca.h>
    #include <xci.h>
    #include <nax0.h>
    #include <extkeys.h>
    #include <packages.h>
    #include <nso.h>
}

namespace gleaf::nsp
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
