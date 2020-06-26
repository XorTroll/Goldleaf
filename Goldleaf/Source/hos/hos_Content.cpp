
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
#include <fs/fs_FileSystem.hpp>

namespace hos
{
    String ContentIdAsString(const NcmContentId &NCAId)
    {
        char idstr[FS_MAX_PATH] = {0};
        u64 lower = __bswap64(*(u64*)NCAId.c);
        u64 upper = __bswap64(*(u64*)(NCAId.c + 0x8));
        snprintf(idstr, FS_MAX_PATH, "%016lx%016lx", lower, upper);
        return String(idstr);
    }

    NcmContentId StringAsContentId(String NCAId)
    {
        NcmContentId nid = {0};
        char lower[0x20] = {0};
        char upper[0x20] = {0};
        memcpy(lower, NCAId.AsUTF8().c_str(), 16);
        memcpy(upper, NCAId.AsUTF8().c_str() + 16, 16);
        *(u64*)nid.c = __bswap64(strtoul(lower, nullptr, 16));
        *(u64*)(nid.c + 0x8) = __bswap64(strtoul(upper, nullptr, 16));
        return nid;
    }

    bool GetPendingUpdateInfo(PendingUpdateVersion *out)
    {
        auto sys = fs::GetNANDSystemExplorer();
        auto ncas = sys->GetFiles("Contents/placehld");
        bool found = false;
        for(auto &nca: ncas)
        {
            std::string path = "@SystemContent://placehld/" + nca.AsUTF8();
            path.reserve(FS_MAX_PATH);
            FsFileSystem ncafs;
            auto rc = fsOpenFileSystemWithId(&ncafs, 0, FsFileSystemType_ContentMeta, path.c_str());
            if(R_SUCCEEDED(rc))
            {
                fs::FspExplorer fwfs("...", ncafs);
                auto fs = fwfs.GetContents();
                for(auto &f: fs)
                {
                    u32 rawver = 0;
                    fwfs.StartFile(f, fs::FileMode::Read);
                    fwfs.ReadFileBlock(f, 8, sizeof(rawver), &rawver);
                    fwfs.EndFile(fs::FileMode::Read);
                    out->Major = (u8)((rawver >> 26) & 0x3f);
                    out->Minor = (u8)((rawver >> 20) & 0x3f);
                    out->Micro = (u8)((rawver >> 16) & 0x3f);
                    found = true;
                    break; // We just want to read the first CNMT NCA we succeed mounting :P
                }
                if(found) break;
            }
        }
        return found;
    }

    SetSysFirmwareVersion ConvertPendingUpdateVersion(PendingUpdateVersion ver)
    {
        SetSysFirmwareVersion fwver = {};
        fwver.major = ver.Major;
        fwver.minor = ver.Minor;
        fwver.micro = ver.Micro;
        sprintf(fwver.display_version, "%d.%d.%d", ver.Major, ver.Minor, ver.Micro);
        return fwver;
    }

    void CleanPendingUpdate()
    {
        auto sys = fs::GetNANDSystemExplorer();
        sys->DeleteDirectory("Contents/placehld");
        sys->CreateDirectory("Contents/placehld");
        nssuInitialize();
        nssuDestroySystemUpdateTask();
        nssuExit();
    }
}