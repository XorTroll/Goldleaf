
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

#include <dump/dump_Export.hpp>
#include <hos/hos_Titles.hpp>
#include <fatfs/fatfs.hpp>
#include <es/es_Service.hpp>

FsStorage g_FatFsDumpBisStorage;

namespace dump {

    void DecryptCopyNAX0ToNCA(NcmContentStorage *cnt_storage, const NcmContentId cnt_id, const std::string &path, std::function<void(double Done, double Total)> cb_fn) {
        s64 cnt_size = 0;
        ncmContentStorageGetSizeFromContentId(cnt_storage, &cnt_size, &cnt_id);
        auto rem_size = static_cast<u64>(cnt_size);
        auto exp = fs::GetExplorerForPath(path);

        auto data_buf = fs::GetWorkBuffer();
        s64 off = 0;
        exp->StartFile(path, fs::FileMode::Write);
        while(rem_size) {
            const auto read_size = std::min(fs::WorkBufferSize, rem_size);
            if(R_FAILED(ncmContentStorageReadContentIdFile(cnt_storage, data_buf, read_size, &cnt_id, off))) {
                break;
            }
            exp->WriteFile(path, data_buf, read_size);
            rem_size -= read_size;
            off += read_size;
            cb_fn((double)off, (double)cnt_size);
        }
        exp->EndFile();
    }

    bool GetMetaRecord(NcmContentMetaDatabase *cnt_meta_db, const u64 app_id, NcmContentMetaKey *out) {
        auto meta_key_array = new NcmContentMetaKey[hos::MaxTitleCount]();
        s32 total = 0;
        s32 written = 0;
        auto got = false;
        auto rc = ncmContentMetaDatabaseList(cnt_meta_db, &total, &written, meta_key_array, hos::MaxTitleCount, NcmContentMetaType_Unknown, app_id, 0, UINT64_MAX, NcmContentInstallType_Full);
        if(R_SUCCEEDED(rc) && (written > 0)) {
            for(s32 i = 0; i < written; i++) {
                if(meta_key_array[i].id == app_id) {
                    *out = meta_key_array[i];
                    got = true;
                    break;
                }
            }
        }
        delete[] meta_key_array;
        return got;
    }

    NcmStorageId GetApplicationLocation(const u64 app_id) {
        NcmContentMetaDatabase cnt_meta_db;
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, NcmStorageId_SdCard))) {
            NcmContentMetaKey temp_meta_key;
            if(GetMetaRecord(&cnt_meta_db, app_id, &temp_meta_key)) {
                return NcmStorageId_SdCard;
            }
            else {
                ncmContentMetaDatabaseClose(&cnt_meta_db);
                if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, NcmStorageId_BuiltInUser))) {
                    if(GetMetaRecord(&cnt_meta_db, app_id, &temp_meta_key)) {
                        return NcmStorageId_BuiltInUser;
                    }
                }
            }
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }
        return NcmStorageId_None;
    }

    void GenerateTicketCert(const u64 app_id) {
        if(R_SUCCEEDED(fsOpenBisStorage(&g_FatFsDumpBisStorage, FsBisPartitionId_System))) {
            auto exp = fs::GetSdCardExplorer();
            FATFS fs;
            f_mount(&fs, "0", 1);
            FIL save;
            f_open(&save, "0:save/80000000000000e1", FA_READ | FA_OPEN_EXISTING);
            std::string title_key;
            std::string out_rights_id;
            const auto &format_app_id = hos::FormatApplicationId(app_id);
            const auto &outdir = GLEAF_PATH_DUMP_TITLE_DIR "/" + format_app_id;
            u32 tmp_size = 0;
            while(true) {
                if(!title_key.empty()) {
                    break;
                }
                auto data_buf = fs::GetWorkBuffer();
                const auto fr = f_read(&save, data_buf, 0x40000, &tmp_size);
                if(fr) {
                    break;
                }
                if(tmp_size == 0) {
                    break;
                }
                for(u32 i = 0; i < tmp_size; i += 0x4000) {
                    if(!title_key.empty()) break;
                    for(u32 j = 0; j < (i + 0x4000); j += 0x400) {
                        if(!title_key.empty()) break;
                        if(hos::IsValidTicketSignature(*reinterpret_cast<u32*>(&data_buf[j]))) {
                            std::stringstream rights_id_strm;
                            for(u32 k = 0; k < 0x10; k++) {
                                const u32 off = j + 0x2a0 + k;
                                rights_id_strm << std::setw(2) << std::setfill('0') << std::hex << (int)data_buf[off];
                            }
                            std::stringstream storage_id_strm;
                            for(u32 k = 0; k < 0x8; k++) {
                                u32 off = j + 0x2a0 + k;
                                storage_id_strm << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)data_buf[off];
                            }
                            std::stringstream title_key_strm;
                            for(u32 k = 0; k < 0x10; k++) {
                                u32 off = j + 0x180 + k;
                                title_key_strm << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << (int)data_buf[off];
                            }
                            const auto &read_app_id = storage_id_strm.str();
                            const auto &rights_id = rights_id_strm.str();
                            const auto &read_title_key = title_key_strm.str();
                            if(format_app_id == read_app_id) {
                                out_rights_id = rights_id;
                                const auto &tik_file = outdir + "/" + rights_id + ".tik";
                                exp->WriteFile(tik_file, &data_buf[j], 0x400);
                                title_key = read_title_key;
                                break;
                            }
                        }
                    }
                }
            }
            f_close(&save);
            f_mount(nullptr, "0", 1);
            fsStorageClose(&g_FatFsDumpBisStorage);
            if(!title_key.empty()) {
                const auto &cert_file = outdir + "/" + out_rights_id + ".cert";
                exp->WriteFile(cert_file, const_cast<u8*>(es::CommonCertificateData), es::CommonCertificateSize);
            }
        }
    }

    std::string GetContentIdPath(NcmContentStorage *cnt_storage, const NcmContentId cnt_id) {
        char out[FS_MAX_PATH] = {};
        if(R_SUCCEEDED(ncmContentStorageGetPath(cnt_storage, out, FS_MAX_PATH, &cnt_id))) {
            return out;
        }
        return "";
    }

    bool GetContentId(NcmContentMetaDatabase *cnt_meta_db, const NcmContentMetaKey *cnt_meta_key, const u64 app_id, const NcmContentType cnt_type, NcmContentId *out) {
        return R_SUCCEEDED(ncmContentMetaDatabaseGetContentIdByType(cnt_meta_db, out, cnt_meta_key, cnt_type));
    }

}