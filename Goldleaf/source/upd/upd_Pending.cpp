
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#include <upd/upd_Pending.hpp>
#include <fs/fs_FileSystem.hpp>

namespace upd {

    bool GetPendingUpdateInfo(PendingUpdateVersion *out) {
        auto sys = fs::GetNANDSystemExplorer();
        const auto ncas = sys->GetFiles("Contents/placehld");
        auto found = false;
        for(const auto &nca: ncas) {
            char path[FS_MAX_PATH] = {};
            snprintf(path, sizeof(path), "@SystemContent://placehld/%s", nca.c_str());
            FsFileSystem cnt_fs;
            // Just read the first CNMT NCA we succeed mounting
            if(R_SUCCEEDED(fsOpenFileSystemWithId(&cnt_fs, 0, FsFileSystemType_ContentMeta, path, FsContentAttributes_All))) {
                fs::FspExplorer test_cnt_exp(cnt_fs, "hos.SystemContentAny");
                const auto fs = test_cnt_exp.GetContents();
                // Read version from the first found file (will be the NCA's CNMT)
                for(const auto &f: fs) {
                    NcmExtPackagedContentMetaHeader cnmt_header;
                    test_cnt_exp.ReadFile(f, 0, sizeof(cnmt_header), &cnmt_header);
                    out->major = (u8)((cnmt_header.version >> 26) & 0x3F);
                    out->minor = (u8)((cnmt_header.version >> 20) & 0x3F);
                    out->micro = (u8)((cnmt_header.version >> 16) & 0x3F);
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

    SetSysFirmwareVersion ConvertPendingUpdateVersion(const PendingUpdateVersion ver) {
        SetSysFirmwareVersion fw_ver = {
            .major = static_cast<u8>(ver.major),
            .minor = static_cast<u8>(ver.minor),
            .micro = static_cast<u8>(ver.micro)
        };
        snprintf(fw_ver.display_version, sizeof(fw_ver.display_version), "%d.%d.%d", ver.major, ver.minor, ver.micro);
        return fw_ver;
    }

    void CleanPendingUpdate() {
        if(R_SUCCEEDED(nssuInitialize())) {
            nssuDestroySystemUpdateTask();
            nssuExit();
        }

        auto sys_exp = fs::GetNANDSystemExplorer();
        sys_exp->EmptyDirectory("Contents/placehld");
    }
 
}
