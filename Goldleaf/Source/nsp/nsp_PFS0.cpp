
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <nsp/nsp_PFS0.hpp>
#include <cstring>

namespace nsp
{
    PFS0::PFS0(fs::Explorer *Exp, String Path)
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
            this->stringtable = new u8[this->header.StringTableSize];
            this->headersize = strtoff + this->header.StringTableSize;
            Exp->ReadFileBlock(this->path, strtoff, this->header.StringTableSize, this->stringtable);
            this->files.reserve(this->header.FileCount);
            for(u32 i = 0; i < this->header.FileCount; i++)
            {
                u64 offset = sizeof(PFS0Header) + (i * sizeof(PFS0FileEntry));
                PFS0FileEntry ent;
                memset(&ent, 0, sizeof(ent));
                Exp->ReadFileBlock(this->path, offset, sizeof(ent), (u8*)&ent);
                String name;
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
        delete[] this->stringtable;
    }

    u32 PFS0::GetCount()
    {
        return this->header.FileCount;
    }

    String PFS0::GetFile(u32 Index)
    {
        return this->files[Index].Name;
    }

    u64 PFS0::ReadFromFile(u32 Index, u64 Offset, u64 Size, u8 *Out)
    {
        return this->gexp->ReadFileBlock(this->path, (this->headersize + this->files[Index].Entry.Offset + Offset), Size, Out);
    }

    std::vector<String> PFS0::GetFiles()
    {
        std::vector<String> pfiles;
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

    void PFS0::SaveFile(u32 Index, fs::Explorer *Exp, String Path)
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

    u32 PFS0::GetFileIndexByName(String File)
    {
        u32 idx = 0;
        for(u32 i = 0; i < this->files.size(); i++)
        {
            if(strcasecmp(this->files[i].Name.AsUTF8().c_str(), File.AsUTF8().c_str()) == 0)
            {
                idx = i;
                break;
            }
        }
        return idx;
    }
}