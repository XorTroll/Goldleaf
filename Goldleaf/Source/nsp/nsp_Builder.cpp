
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

#include <nsp/nsp_Builder.hpp>
#include <fs/fs_Explorer.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

namespace nsp
{
    bool GenerateFrom(pu::String Input, pu::String Out, std::function<void(u64, u64)> Callback)
    {
        auto exp = fs::GetExplorerForPath(Input);
        auto files = exp->GetFiles(Input);
        PFS0Header header = {};
        header.FileCount = (u32)files.size();
        header.Magic = Magic;
        u8 *strtable = fs::GetFileSystemOperationsBuffer();
        size_t strtablesize = 0;
        std::vector<PFS0File> fentries;
        size_t base_offset = 0;
        for(auto &file: files)
        {
            PFS0File entry = {};
            entry.Entry.Offset = base_offset;
            entry.Entry.StringTableOffset = strtablesize;
            auto fsize = exp->GetFileSize(Input + "/" + file);
            entry.Entry.Size = fsize;
            entry.Name = file;
            base_offset += fsize;
            strcpy((char*)&strtable[strtablesize], file.AsUTF8().c_str());
            strtablesize += file.length() + 1; // NUL terminated!
            fentries.push_back(entry);
        }
        strtablesize = (strtablesize + 0x1f) &~ 0x1f;
        header.StringTableSize = strtablesize;
        auto outexp = fs::GetExplorerForPath(Out);
        outexp->WriteFileBlock(Out, (u8*)&header, sizeof(PFS0Header));
        for(auto &entry: fentries)
        {
            outexp->WriteFileBlock(Out, (u8*)&entry.Entry, sizeof(PFS0FileEntry));
        }
        outexp->WriteFileBlock(Out, strtable, strtablesize);
        size_t done = 0;
        for(auto &entry: fentries)
        {
            size_t toread = entry.Entry.Size;
            u8 *buf = fs::GetFileSystemOperationsBuffer();
            size_t readsz = fs::GetFileSystemOperationsBufferSize();
            size_t fdone = 0;
            while(toread)
            {
                auto read = exp->ReadFileBlock(Input + "/" + entry.Name, fdone, std::min(toread, readsz), buf);
                outexp->WriteFileBlock(Out, buf, read);
                fdone += read;
                done += read;
                toread -= read;
                Callback(done, base_offset);
            }
        }
        return true;
    }
}