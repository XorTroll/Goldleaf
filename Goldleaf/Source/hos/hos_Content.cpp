
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

#include <hos/hos_Content.hpp>
#include <fs/fs_Explorer.hpp>

namespace hos
{
    pu::String ContentIdAsString(const NcmNcaId &NCAId)
    {
        char idstr[FS_MAX_PATH] = {0};
        u64 lower = __bswap64(*(u64*)NCAId.c);
        u64 upper = __bswap64(*(u64*)(NCAId.c + 0x8));
        snprintf(idstr, FS_MAX_PATH, "%016lx%016lx", lower, upper);
        return pu::String(idstr);
    }

    NcmNcaId StringAsContentId(pu::String NCAId)
    {
        NcmNcaId nid = {0};
        char lower[0x20] = {0};
        char upper[0x20] = {0};
        memcpy(lower, NCAId.AsUTF8().c_str(), 16);
        memcpy(upper, NCAId.AsUTF8().c_str() + 16, 16);
        *(u64*)nid.c = __bswap64(strtoul(lower, NULL, 16));
        *(u64*)(nid.c + 0x8) = __bswap64(strtoul(upper, NULL, 16));
        return nid;
    }

    SetSysFirmwareVersion GetPendingUpdateInfo()
    {
        auto sys = fs::GetNANDSystemExplorer();
        auto ncas = sys->GetFiles("Contents/placehld");
        SetSysFirmwareVersion fwver = {0};
        for(auto &nca: ncas)
        {
            std::string path = "@SystemContent://placehld/" + nca.AsUTF8();
            path.reserve(FS_MAX_PATH);
            FsFileSystem ncafs;
            auto rc = fsOpenFileSystemWithId(&ncafs, 0x0100000000000809, FsFileSystemType_ContentData, path.c_str());
            if(R_SUCCEEDED(rc))
            {
                fs::FileSystemExplorer fwfs("gncafwver", "...", &ncafs);
                fwfs.ReadFileBlock("file", 0, sizeof(fwver), (u8*)&fwver);
                break;
            }
        }
        return fwver;
    }

    void CleanPendingUpdate()
    {
        auto sys = fs::GetNANDSystemExplorer();
        sys->DeleteDirectory("Contents/placehld");
        sys->CreateDirectory("Contents/placehld");
    }
}