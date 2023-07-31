
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

#include <expt/expt_Export.hpp>
#include <hos/hos_Titles.hpp>
#include <fatfs/fatfs.hpp>
#include <es/es_Service.hpp>
#include <cfg/cfg_Settings.hpp>

extern cfg::Settings g_Settings;

FsStorage g_FatFsDumpBisStorage;

namespace expt {

    namespace {

        hos::TicketFile ReadTicket(const u64 app_id) {
            hos::TicketFile read_tik_file = {};

            if(R_SUCCEEDED(fsOpenBisStorage(&g_FatFsDumpBisStorage, FsBisPartitionId_System))) {
                FATFS fs;
                GLEAF_ASSERT_TRUE(f_mount(&fs, "0", 1) == FR_OK);
                FIL save;
                GLEAF_ASSERT_TRUE(f_open(&save, "0:/save/80000000000000e1", FA_READ | FA_OPEN_EXISTING) == FR_OK);

                u32 tmp_size = 0;
                u8 tmp_tik_buf[0x400];
                while(true) {
                    const auto fr = f_read(&save, tmp_tik_buf, sizeof(tmp_tik_buf), &tmp_size);
                    if(fr != FR_OK) {
                        break;
                    }
                    if(tmp_size == 0) {
                        break;
                    }

                    const auto tik_sig = *reinterpret_cast<hos::TicketSignature*>(tmp_tik_buf);
                    if(hos::IsValidTicketSignature(tik_sig)) {
                        hos::TicketFile tik_file = { .signature = tik_sig };
                        const auto tik_sig_size = hos::GetTicketSignatureSize(tik_file.signature);
                        memcpy(tik_file.signature_data, tmp_tik_buf + sizeof(tik_file.signature), hos::GetTicketSignatureDataSize(tik_file.signature));
                        memcpy(&tik_file.data, tmp_tik_buf + tik_sig_size, sizeof(tik_file.data));

                        if(app_id == tik_file.data.rights_id.GetApplicationId()) {
                            read_tik_file = tik_file;
                            break;
                        }
                    }
                }

                f_close(&save);
                f_mount(nullptr, "0", 1);
                fsStorageClose(&g_FatFsDumpBisStorage);
            }

            return read_tik_file;
        }

    }

    Result DecryptCopyNAX0ToNCA(NcmContentStorage *cnt_storage, const NcmContentId cnt_id, const std::string &path, DecryptStartCallback dec_start_cb, DecryptProgressCallback dec_prog_cb) {
        s64 cnt_size = 0;
        GLEAF_RC_TRY(ncmContentStorageGetSizeFromContentId(cnt_storage, &cnt_size, &cnt_id));
        auto rem_size = static_cast<u64>(cnt_size);
        auto exp = fs::GetExplorerForPath(path);
        dec_start_cb((double)cnt_size);

        auto data_buf = new u8[g_Settings.decrypt_buffer_max_size]();
        ScopeGuard on_exit([&]() {
            delete[] data_buf;
        });

        s64 off = 0;
        exp->StartFile(path, fs::FileMode::Write);
        while(rem_size) {
            const auto read_size = std::min(g_Settings.decrypt_buffer_max_size, rem_size);
            GLEAF_RC_TRY(ncmContentStorageReadContentIdFile(cnt_storage, data_buf, read_size, &cnt_id, off));
            exp->WriteFile(path, data_buf, read_size);
            rem_size -= read_size;
            off += read_size;
            dec_prog_cb((double)read_size);
        }
        exp->EndFile();
        
        GLEAF_RC_SUCCEED;
    }

    std::string ExportTicketCert(const u64 app_id, const bool export_cert) {
        const auto tik_file = ReadTicket(app_id);

        std::stringstream rights_id_strm;
        for(u32 i = 0; i < sizeof(tik_file.data.rights_id.id); i++) {
            rights_id_strm << std::setw(2) << std::setfill('0') << std::hex << static_cast<u32>(tik_file.data.rights_id.id[i]);
        }
        const auto fmt_rights_id = rights_id_strm.str();

        const auto fmt_app_id = hos::FormatApplicationId(tik_file.data.rights_id.GetApplicationId());

        auto exp = fs::GetSdCardExplorer();
        const auto &out_dir = GLEAF_PATH_EXPORT_TITLE_DIR "/" + fmt_app_id;
        exp->CreateDirectory(out_dir);

        const auto &tik_path = out_dir + "/" + fmt_rights_id + ".tik";
        hos::SaveTicket(exp, tik_path, tik_file);

        if(export_cert) {
            const auto &cert_path = out_dir + "/" + fmt_rights_id + ".cert";
            exp->WriteFile(cert_path, const_cast<u8*>(es::CommonCertificateData), es::CommonCertificateSize);
        }

        return tik_path;
    }

    std::string GetContentIdPath(NcmContentStorage *cnt_storage, const NcmContentId cnt_id) {
        char out[FS_MAX_PATH] = {};
        ncmContentStorageGetPath(cnt_storage, out, FS_MAX_PATH, &cnt_id);
        return out;
    }

}