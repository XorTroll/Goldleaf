
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

namespace hos {

    String ContentIdAsString(const NcmContentId &cnt_id) {
        char cnt_id_str[FS_MAX_PATH] = {0};
        const auto lower = __bswap64(*(u64*)cnt_id.c);
        const auto upper = __bswap64(*(u64*)(cnt_id.c + 0x8));
        snprintf(cnt_id_str, FS_MAX_PATH, "%016lx%016lx", lower, upper);
        return cnt_id_str;
    }

    NcmContentId StringAsContentId(String cnt_id_str) {
        NcmContentId cnt_id = {};
        char lower[0x20] = {0};
        char upper[0x20] = {0};
        memcpy(lower, cnt_id_str.AsUTF8().c_str(), 0x10);
        memcpy(upper, cnt_id_str.AsUTF8().c_str() + 0x10, 0x10);
        *(u64*)cnt_id.c = __bswap64(strtoul(lower, nullptr, 0x10));
        *(u64*)(cnt_id.c + 0x8) = __bswap64(strtoul(upper, nullptr, 0x10));
        return cnt_id;
    }

    bool GetPendingUpdateInfo(PendingUpdateVersion *out) {
        auto sys = fs::GetNANDSystemExplorer();
        auto ncas = sys->GetFiles("Contents/placehld");
        bool found = false;
        for(auto &nca: ncas) {
            auto path = "@SystemContent://placehld/" + nca.AsUTF8();
            path.reserve(FS_MAX_PATH);
            FsFileSystem ncafs;
            // Just read the first CNMT NCA we succeed mounting
            if(R_SUCCEEDED(fsOpenFileSystemWithId(&ncafs, 0, FsFileSystemType_ContentMeta, path.c_str()))) {
                fs::FspExplorer fwfs("...", ncafs);
                const auto fs = fwfs.GetContents();
                for(const auto &f: fs) {
                    u32 raw_ver = 0;
                    fwfs.ReadFile(f, 0x8, sizeof(raw_ver), &raw_ver);
                    out->major = (u8)((raw_ver >> 26) & 0x3f);
                    out->minor = (u8)((raw_ver >> 20) & 0x3f);
                    out->micro = (u8)((raw_ver >> 16) & 0x3f);
                    found = true;
                    break; 
                }
                if(found) {
                    break;
                }
            }
        }
        return found;
    }

    SetSysFirmwareVersion ConvertPendingUpdateVersion(PendingUpdateVersion ver) {
        SetSysFirmwareVersion fwver = {};
        fwver.major = ver.major;
        fwver.minor = ver.minor;
        fwver.micro = ver.micro;
        sprintf(fwver.display_version, "%d.%d.%d", ver.major, ver.minor, ver.micro);
        return fwver;
    }

    void CleanPendingUpdate() {
        auto sys_exp = fs::GetNANDSystemExplorer();
        sys_exp->EmptyDirectory("Contents/placehld");

        if(R_SUCCEEDED(nssuInitialize())) {
            nssuDestroySystemUpdateTask();
            nssuExit();
        }
    }
 
}