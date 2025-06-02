
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

#include <upd/upd_Update.hpp>
#include <fs/fs_FileSystem.hpp>
#include <cnt/cnt_PackagedContentMeta.hpp>

namespace upd {

    namespace {

        bool GetUpdateInfo(const char *update_dir, UpdateVersion &out_ver) {
            auto sys = fs::GetNANDSystemExplorer();
            const auto ncas = sys->GetFiles(std::string("Contents/") + update_dir);
            auto found = false;
            for(const auto &nca: ncas) {
                char path[FS_MAX_PATH] = {};
                snprintf(path, sizeof(path), "@SystemContent://%s/%s", update_dir, nca.c_str());
                FsFileSystem cnt_fs;
                if(R_SUCCEEDED(fsOpenFileSystemWithId(&cnt_fs, 0, FsFileSystemType_ContentMeta, path, FsContentAttributes_All))) {
                    fs::FspExplorer test_cnt_exp(cnt_fs, "hos.UpdateContent");
                    const auto fs = test_cnt_exp.GetContents();
                    // Read version from the first file we can (will be the NCA's CNMT)
                    for(const auto &f: fs) {
                        if(f.substr(f.length() - __builtin_strlen(".cnmt")) == ".cnmt") {
                            cnt::PackagedContentMeta cnmt = {};
                            const auto cnmt_size = test_cnt_exp.GetFileSize(f);
                            auto cnmt_buf = fs::AllocateWorkBuffer(cnmt_size);

                            if(test_cnt_exp.ReadFile(f, 0, cnmt_size, cnmt_buf) == cnmt_size) {
                                if(cnt::ReadContentMeta(cnmt_buf, cnmt_size, cnmt)) {
                                    // https://switchbrew.org/wiki/Title_list#System_Modules
                                    out_ver.major = (u8)((cnmt.header.version >> 26) & 0b111111); // bits 26-31
                                    out_ver.minor = (u8)((cnmt.header.version >> 20) & 0b111111); // bits 20-25
                                    out_ver.micro = (u8)((cnmt.header.version >> 16) & 0b1111); // bits 16-19
                                    out_ver.relstep = (u16)(cnmt.header.version & UINT16_MAX); // bits 0-15

                                    GLEAF_LOG_FMT("Found pending system update version 0x%X %d.%d.%d (%d)", cnmt.header.version, out_ver.major, out_ver.minor, out_ver.micro, out_ver.relstep);
                                    found = true;
                                    break;
                                }
                                else {
                                    GLEAF_LOG_FMT("Failed to parse CNMT from '%s'", f.c_str());
                                }
                            }
                            else {
                                GLEAF_LOG_FMT("Failed to read CNMT file '%s'", f.c_str());
                            }
                        }
                    }
                    if(found) {
                        break;
                    }
                }
            }
            return found;
        }

    }

    bool GetPendingUpdateInfo(UpdateVersion &out_ver) {
        return GetUpdateInfo("placehld", out_ver);
    }

    void CleanPendingUpdate() {
        if(R_SUCCEEDED(nssuInitialize())) {
            nssuDestroySystemUpdateTask();
            nssuExit();
        }

        auto sys_exp = fs::GetNANDSystemExplorer();
        sys_exp->EmptyDirectory("Contents/placehld");
    }

    SetSysFirmwareVersion GetUpdateFirmwareVersion(const UpdateVersion &ver) {
        SetSysFirmwareVersion fw_ver = {
            .major = static_cast<u8>(ver.major),
            .minor = static_cast<u8>(ver.minor),
            .micro = static_cast<u8>(ver.micro)
        };
        snprintf(fw_ver.display_version, sizeof(fw_ver.display_version), "%d.%d.%d", ver.major, ver.minor, ver.micro);
        return fw_ver;
    }
 
}
