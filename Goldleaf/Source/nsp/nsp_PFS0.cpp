#include <nsp/nsp_PFS0.hpp>
#include <cstring>

namespace nsp
{
    PFS0::PFS0(fs::Explorer *Exp, std::string Path)
    {
        this->path = Path;
        this->gexp = Exp;
        this->ok = false;
        this->headersize = 0;
        Exp->ReadFileBlock(this->path, 0, sizeof(this->header), (u8*)&this->header);
        if(this->header.Magic == Magic)
        {
            this->ok = true;
            u64 strtoff = sizeof(PFS0Header) + (sizeof(PFS0FileEntry) * this->header.FileCount);
            this->stringtable = (u8*)malloc(this->header.StringTableSize);
            this->headersize = strtoff + this->header.StringTableSize;
            Exp->ReadFileBlock(this->path, strtoff, this->header.StringTableSize, this->stringtable);
            this->files.reserve(this->header.FileCount);
            for(u32 i = 0; i < this->header.FileCount; i++)
            {
                u64 offset = sizeof(PFS0Header) + (i * sizeof(PFS0FileEntry));
                PFS0FileEntry ent;
                memset(&ent, 0, sizeof(ent));
                Exp->ReadFileBlock(this->path, offset, sizeof(ent), (u8*)&ent);
                std::string name;
                for(u32 i = ent.StringTableOffset; i < this->header.StringTableSize; i++)
                {
                    char ch = (char)this->stringtable[i];
                    if(ch == '\0') break;
                    name += ch;
                }
                PFS0File fl;
                fl.Entry = ent;
                fl.Name = name;
                this->files.push_back(fl);
            }
        }
    }

    PFS0::~PFS0()
    {
        free(this->stringtable);
    }

    u32 PFS0::GetCount()
    {
        return this->header.FileCount;
    }

    std::string PFS0::GetFile(u32 Index)
    {
        return this->files[Index].Name;
    }

    u64 PFS0::ReadFromFile(u32 Index, u64 Offset, u64 Size, u8 *Out)
    {
        return this->gexp->ReadFileBlock(this->path, (this->headersize + this->files[Index].Entry.Offset + Offset), Size, Out);
    }

    std::vector<std::string> PFS0::GetFiles()
    {
        std::vector<std::string> pfiles;
        for(u32 i = 0; i < this->files.size(); i++) pfiles.push_back(this->files[i].Name);
        return pfiles;
    }

    bool PFS0::IsOk()
    {
        return this->ok;
    }

    fs::Explorer *PFS0::GetExplorer()
    {
        return this->gexp;
    }

    u64 PFS0::GetFileSize(u32 Index)
    {
        return this->files[Index].Entry.Size;
    }

    void PFS0::SaveFile(u32 Index, fs::Explorer *Exp, std::string Path)
    {
        u64 fsize = this->GetFileSize(Index);
        u64 rsize = fs::GetFileSystemOperationsBufferSize();
        u8 *bdata = fs::GetFileSystemOperationsBuffer();
        u64 szrem = fsize;
        u64 off = 0;
        Exp->DeleteFile(Path);
        Exp->CreateFile(Path);
        while(szrem)
        {
            u64 tread = std::min(rsize, szrem);
            u64 rbytes = this->ReadFromFile(Index, off, tread, bdata);
            Exp->WriteFileBlock(Path, bdata, rbytes);
            off += rbytes;
            szrem -= rbytes;
        }
    }

    u32 PFS0::GetFileIndexByName(std::string File)
    {
        u32 idx = 0;
        for(u32 i = 0; i < this->files.size(); i++)
        {
            if(strcasecmp(this->files[i].Name.c_str(), File.c_str()) == 0)
            {
                idx = i;
                break;
            }
        }
        return idx;
    }
}