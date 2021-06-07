
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

#include <nsp/nsp_Installer.hpp>
#include <err/err_Result.hpp>
#include <fs/fs_FileSystem.hpp>
#include <hos/hos_Common.hpp>

extern cfg::Settings g_Settings;

namespace nsp {

    Installer::~Installer() {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation() {
        ERR_RC_UNLESS(pfs0_file.IsOk(), err::result::ResultInvalidNSP);
        ERR_RC_TRY(ncmOpenContentStorage(&this->cnt_storage, this->storage_id));
        ERR_RC_TRY(ncmOpenContentMetaDatabase(&this->cnt_meta_db, this->storage_id));

        String cnmt_nca_file_name;
        auto cnmt_nca_file_idx = PFS0::InvalidFileIndex;
        u64 cnmt_nca_file_size = 0;
        auto tik_file_idx = PFS0::InvalidFileIndex;
        tik_file_size = 0;
        auto pfs0_files = pfs0_file.GetFiles();
        for(u32 i = 0; i < pfs0_files.size(); i++) {
            auto file = pfs0_files[i];
            if(fs::GetExtension(file) == "tik") {
                tik_file_name = file;
                tik_file_idx = i;
                tik_file_size = pfs0_file.GetFileSize(i);
            }
            else {
                if(file.length() >= 8) {
                    if(file.substr(file.length() - 8) == "cnmt.nca") {
                        cnmt_nca_file_name = file;
                        cnmt_nca_file_idx = i;
                        cnmt_nca_file_size = pfs0_file.GetFileSize(i);
                    }
                }
            }
        }
        ERR_RC_UNLESS(PFS0::IsValidFileIndex(cnmt_nca_file_idx), err::result::ResultMetaNotFound);
        ERR_RC_UNLESS(cnmt_nca_file_size > 0, err::result::ResultMetaNotFound);
        auto cnmt_nca_content_id = fs::GetFileName(cnmt_nca_file_name);

        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        nand_sys_explorer->CreateDirectory("Contents/temp");

        if(tik_file_size > 0) {
            auto tik_path = nand_sys_explorer->FullPathFor("Contents/temp/" + tik_file_name);
            this->pfs0_file.SaveFile(tik_file_idx, nand_sys_explorer, tik_path);
            this->tik_file = hos::ReadTicket(tik_path);
        }

        auto cnmt_nca_temp_path = nand_sys_explorer->FullPathFor("Contents/temp/" + cnmt_nca_file_name);
        nand_sys_explorer->DeleteFile(cnmt_nca_temp_path);
        pfs0_file.SaveFile(cnmt_nca_file_idx, nand_sys_explorer, cnmt_nca_temp_path);

        char cnmt_nca_content_path[FS_MAX_PATH] = {};
        sprintf(cnmt_nca_content_path, "@SystemContent://temp/%s", cnmt_nca_file_name.AsUTF8().c_str());
        FsRightsId tmp_rid;
        ERR_RC_TRY(fsGetRightsIdAndKeyGenerationByPath(cnmt_nca_content_path, &keygen, &tmp_rid));
        const auto system_keygen = hos::ComputeSystemKeyGeneration();
        ERR_RC_UNLESS(system_keygen >= keygen, err::result::ResultKeyGenMismatch);

        FsFileSystem cnmt_nca_fs;
        ERR_RC_TRY(fsOpenFileSystemWithId(&cnmt_nca_fs, 0, FsFileSystemType_ContentMeta, cnmt_nca_content_path));
        
        {
            fs::FspExplorer cnmt_nca_fs_obj("NSP-ContentMeta", cnmt_nca_fs);
            String cnmt_file_name;
            for(auto &cnt: cnmt_nca_fs_obj.GetContents()) {
                if(fs::GetExtension(cnt) == "cnmt") {
                    cnmt_file_name = cnt;
                    break;
                }
            }
            ERR_RC_UNLESS(!cnmt_file_name.empty(), err::result::ResultMetaNotFound);
            const auto cnmt_file_size = cnmt_nca_fs_obj.GetFileSize(cnmt_file_name);
            auto cnmt_tmp_buf = fs::GetWorkBuffer();
            cnmt_nca_fs_obj.ReadFile(cnmt_file_name, 0, cnmt_file_size, cnmt_tmp_buf);
            this->cnmt = ncm::ContentMeta(cnmt_tmp_buf, cnmt_file_size);
        }

        NcmContentInfo cnt_info = {
            .content_id = hos::StringAsContentId(cnmt_nca_content_id),
            .content_type = NcmContentType_Meta,
        };
        *reinterpret_cast<u64*>(cnt_info.size) = cnmt_nca_file_size;

        this->cnt_meta_key = cnmt.GetContentMetaKey();
        ERR_RC_UNLESS(!hos::ExistsTitle(NcmContentMetaType_Unknown, NcmStorageId_SdCard, this->cnt_meta_key.id), err::result::ResultTitleAlreadyInstalled);
        ERR_RC_UNLESS(!hos::ExistsTitle(NcmContentMetaType_Unknown, NcmStorageId_BuiltInUser, this->cnt_meta_key.id), err::result::ResultTitleAlreadyInstalled);
        
        bool has_cnmt_installed = false;
        ERR_RC_TRY(ncmContentStorageHas(&this->cnt_storage, &has_cnmt_installed, &cnt_info.content_id));
        if(!has_cnmt_installed) {
            this->contents.push_back(cnt_info);
        }
        this->cnmt.GetInstallContentMeta(this->cnmt_buf, cnt_info, g_Settings.ignore_required_fw_ver);
        this->base_app_id = hos::GetBaseApplicationId(this->cnt_meta_key.id, static_cast<NcmContentMetaType>(this->cnt_meta_key.type));
        this->nacp_data = {};

        for(const auto &cnt: this->cnmt.GetContents()) {
            this->contents.push_back(cnt);
            if(cnt.content_type == NcmContentType_Control) {
                auto control_nca_content_id = hos::ContentIdAsString(cnt.content_id);
                auto control_nca_file_name = control_nca_content_id + ".nca";
                const auto control_nca_file_idx = this->pfs0_file.GetFileIndexByName(control_nca_file_name);
                if(PFS0::IsValidFileIndex(control_nca_file_idx)) {
                    auto control_nca_temp_path = nand_sys_explorer->MakeFull("Contents/temp/" + control_nca_file_name);
                    this->pfs0_file.SaveFile(control_nca_file_idx, nand_sys_explorer, control_nca_temp_path);
                    char control_nca_content_path[FS_MAX_PATH] = {};
                    sprintf(control_nca_content_path, "@SystemContent://temp/%s", control_nca_file_name.AsUTF8().c_str());
                    FsFileSystem control_nca_fs;
                    if(R_SUCCEEDED(fsOpenFileSystemWithId(&control_nca_fs, this->cnt_meta_key.id, FsFileSystemType_ContentControl, control_nca_content_path))) {
                        fs::FspExplorer control_nca_fs_obj("ControlData", control_nca_fs);
                        for(auto &cnt: control_nca_fs_obj.GetContents()) {
                            if(fs::GetExtension(cnt) == "dat") {
                                auto sd_exp = fs::GetSdCardExplorer();
                                this->icon = sd_exp->MakeAbsolute(consts::Root + "/meta/" + control_nca_content_id + ".jpg");
                                control_nca_fs_obj.CopyFile(cnt, this->icon);
                                break;
                            }
                        }
                        control_nca_fs_obj.ReadFile("control.nacp", 0, sizeof(nacp_data), &nacp_data);
                    }
                }
            }
        }
        return err::result::ResultSuccess;
    }

    Result Installer::PreProcessContents() {
        ERR_RC_TRY(ncmContentMetaDatabaseSet(&this->cnt_meta_db, &this->cnt_meta_key, this->cnmt_buf.GetData(), this->cnmt_buf.GetSize()));
        ERR_RC_TRY(ncmContentMetaDatabaseCommit(&this->cnt_meta_db));

        s32 content_meta_count = 0;
        const auto rc = nsCountApplicationContentMeta(this->base_app_id, &content_meta_count);
        if(rc != 0x410) {
            ERR_RC_TRY(rc);
        }

        std::vector<ns::ContentStorageMetaKey> content_storage_meta_keys;
        if(content_meta_count > 0) {
            auto cnt_storage_meta_key_buf = reinterpret_cast<ns::ContentStorageMetaKey*>(fs::GetWorkBuffer());
            u32 real_count = 0;
            ERR_RC_TRY(ns::ListApplicationRecordContentMeta(0, this->base_app_id, cnt_storage_meta_key_buf, content_meta_count * sizeof(ns::ContentStorageMetaKey), &real_count));
            for(u32 i = 0; i < real_count; i++) {
                content_storage_meta_keys.push_back(cnt_storage_meta_key_buf[i]);
            }
        }

        const ns::ContentStorageMetaKey cnt_storage_meta_key = {
            .meta_key = this->cnt_meta_key,
            .storage_id = this->storage_id,
        };
        content_storage_meta_keys.push_back(cnt_storage_meta_key);
        ns::DeleteApplicationRecord(this->base_app_id);
        ERR_RC_TRY(ns::PushApplicationRecord(this->base_app_id, 3, content_storage_meta_keys.data(), content_storage_meta_keys.size() * sizeof(ns::ContentStorageMetaKey)));

        if(this->tik_file_size > 0) {
            auto tmp_buf = fs::GetWorkBuffer();
            auto tik_path = "Contents/temp/" + this->tik_file_name;
            auto nand_sys_explorer = fs::GetNANDSystemExplorer();
            nand_sys_explorer->ReadFile(tik_path, 0, this->tik_file.GetFullSize(), tmp_buf);
            ERR_RC_TRY(es::ImportTicket(tmp_buf, this->tik_file_size, es::CommonCertificateData, es::CommonCertificateSize));
        }
        return err::result::ResultSuccess;
    }

    std::string Installer::GetExportedIconPath() {
        return this->icon.AsUTF8();
    }

    Result Installer::WriteContents(OnContentsWriteFunction on_content_write_cb) {
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        auto tmp_buf = fs::GetWorkBuffer();
        u64 total_size = 0;
        u64 total_written_size = 0;
        std::vector<u32> content_file_idxs;
        for(const auto &cnt: this->contents) {
            auto content_file_name = hos::ContentIdAsString(cnt.content_id);
            if(cnt.content_type == NcmContentType_Meta) {
                content_file_name += ".cnmt";
            }
            content_file_name += ".nca";
            const auto content_file_idx = this->pfs0_file.GetFileIndexByName(content_file_name);
            ERR_RC_UNLESS(PFS0::IsValidFileIndex(content_file_idx), err::result::ResultInvalidNSP);
            total_size += pfs0_file.GetFileSize(content_file_idx);
            content_file_idxs.push_back(content_file_idx);
        }

        for(u32 i = 0; i < this->contents.size(); i++) {
            const auto &cnt = this->contents[i];
            auto content_file_idx = content_file_idxs[i];
            auto content_file_name = this->pfs0_file.GetFile(content_file_idx);
            auto content_file_size = this->pfs0_file.GetFileSize(content_file_idx);

            NcmPlaceHolderId placehld_id = {};
            memcpy(placehld_id.uuid.uuid, cnt.content_id.c, 0x10);
            
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &placehld_id);
            ERR_RC_TRY(ncmContentStorageCreatePlaceHolder(&this->cnt_storage, &cnt.content_id, &placehld_id, content_file_size));
            u64 cur_written_size = 0;
            auto rem_size = content_file_size;
            auto content_path = "Contents/temp/" + content_file_name;
            switch(cnt.content_type) {
                case NcmContentType_Meta:
                case NcmContentType_Control: {
                    nand_sys_explorer->StartFile(content_path, fs::FileMode::Read);
                    break;
                }
                default: {
                    pfs0_file.GetExplorer()->StartFile(pfs0_file.GetPath(), fs::FileMode::Read);
                    break;
                }
            }
            while(rem_size) {
                u64 tmp_read_size = 0;
                const auto read_size = std::min(rem_size, fs::WorkBufferSize);
                const auto time_pre = std::chrono::steady_clock::now();
                switch(cnt.content_type) {
                    case NcmContentType_Meta:
                    case NcmContentType_Control: {
                        tmp_read_size = nand_sys_explorer->ReadFile(content_path, cur_written_size, read_size, tmp_buf);
                        break;
                    }
                    default: {
                        tmp_read_size = pfs0_file.ReadFromFile(content_file_idx, cur_written_size, read_size, tmp_buf);
                        break;
                    }
                }
                ERR_RC_TRY(ncmContentStorageWritePlaceHolder(&this->cnt_storage, &placehld_id, cur_written_size, tmp_buf, tmp_read_size));
                cur_written_size += tmp_read_size;
                rem_size -= tmp_read_size;
                const auto time_post = std::chrono::steady_clock::now();
                const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_post - time_pre).count();
                const auto bytes_per_sec = (1000.0f / (double)(diff)) * (double)(tmp_read_size); // By elapsed time and written bytes, compute how much data has been written in 1 second
                on_content_write_cb(cnt, i, this->contents.size(), (double)(cur_written_size + total_written_size), (double)total_size, (u64)bytes_per_sec);
            }
            switch(cnt.content_type) {
                case NcmContentType_Meta:
                case NcmContentType_Control: {
                    nand_sys_explorer->EndFile();
                    break;
                }
                default: {
                    pfs0_file.GetExplorer()->EndFile();
                    break;
                }
            }
            total_written_size += cur_written_size;
            ERR_RC_TRY(ncmContentStorageRegister(&this->cnt_storage, &cnt.content_id, &placehld_id));
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &placehld_id);
        }
        return err::result::ResultSuccess;
    }

    void Installer::FinalizeInstallation() {
        ncmContentStorageClose(&this->cnt_storage);
        ncmContentMetaDatabaseClose(&this->cnt_meta_db);
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        nand_sys_explorer->DeleteDirectory("Contents/temp");
    }

}