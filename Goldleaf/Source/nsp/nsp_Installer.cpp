
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

#include <nsp/nsp_Installer.hpp>
#include <err/err_Result.hpp>
#include <fs/fs_FileSystem.hpp>
#include <sys/stat.h>
#include <hos/hos_Common.hpp>
#include <fstream>
#include <malloc.h>
#include <dirent.h>
#include <chrono>

extern cfg::Settings global_settings;

namespace nsp
{
    Installer::~Installer()
    {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation()
    {
        ERR_RC_UNLESS(pfs0_file.IsOk(), err::result::ResultInvalidNSP);
        ERR_RC_TRY(ncmOpenContentStorage(&this->cnt_storage, this->storage_id));
        ERR_RC_TRY(ncmOpenContentMetaDatabase(&this->cnt_meta_db, this->storage_id));

        String cnmt_nca_file_name;
        u32 cnmt_nca_file_idx = PFS0::InvalidFileIndex;
        u64 cnmt_nca_file_size = 0;
        u32 tik_file_idx = PFS0::InvalidFileIndex;
        tik_file_size = 0;
        auto pfs0_files = pfs0_file.GetFiles();
        for(u32 i = 0; i < pfs0_files.size(); i++)
        {
            auto file = pfs0_files[i];
            if(fs::GetExtension(file) == "tik")
            {
                tik_file_name = file;
                tik_file_idx = i;
                tik_file_size = pfs0_file.GetFileSize(i);
            }
            else
            {
                if(file.length() >= 8)
                {
                    if(file.substr(file.length() - 8) == "cnmt.nca")
                    {
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
        auto cnmt_nca_temp_path = nand_sys_explorer->FullPathFor("Contents/temp/" + cnmt_nca_file_name);
        nand_sys_explorer->DeleteFile(cnmt_nca_temp_path);
        pfs0_file.SaveFile(cnmt_nca_file_idx, nand_sys_explorer, cnmt_nca_temp_path);

        char cnmt_nca_content_path[FS_MAX_PATH] = {0};
        sprintf(cnmt_nca_content_path, "@SystemContent://temp/%s", cnmt_nca_file_name.AsUTF8().c_str());
        FsRightsId tmp_rid;
        ERR_RC_TRY(fsGetRightsIdAndKeyGenerationByPath(cnmt_nca_content_path, &keygen, &tmp_rid));
        auto systemkgen = hos::ComputeSystemKeyGeneration();
        ERR_RC_UNLESS(systemkgen >= keygen, err::result::ResultKeyGenMismatch);

        FsFileSystem cnmt_nca_fs;
        ERR_RC_TRY(fsOpenFileSystemWithId(&cnmt_nca_fs, 0, FsFileSystemType_ContentMeta, cnmt_nca_content_path));
        {
            fs::FspExplorer cnmt_nca_fs_obj("NSP-ContentMeta", cnmt_nca_fs);
            String cnmt_file_name;
            for(auto &cnt: cnmt_nca_fs_obj.GetContents())
            {
                if(fs::GetExtension(cnt) == "cnmt")
                {
                    cnmt_file_name = cnt;
                    break;
                }
            }
            ERR_RC_UNLESS(!cnmt_file_name.empty(), err::result::ResultMetaNotFound);
            auto cnmt_file_size = cnmt_nca_fs_obj.GetFileSize(cnmt_file_name);
            auto cnmt_tmp_buf = fs::GetWorkBuffer();
            cnmt_nca_fs_obj.StartFile(cnmt_file_name, fs::FileMode::Read);
            cnmt_nca_fs_obj.ReadFileBlock(cnmt_file_name, 0, cnmt_file_size, cnmt_tmp_buf);
            cnmt_nca_fs_obj.EndFile(fs::FileMode::Read);
            this->cnmt = ncm::ContentMeta(cnmt_tmp_buf, cnmt_file_size);
        }

        ncm::ContentRecord record = {};
        record.ContentId = hos::StringAsContentId(cnmt_nca_content_id);
        *(u64*)record.Size = (cnmt_nca_file_size & 0xffffffffffff);
        record.Type = ncm::ContentType::Meta;
        this->cnt_meta_key = cnmt.GetContentMetaKey();
        ERR_RC_UNLESS(!hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, this->cnt_meta_key.id), err::result::ResultTitleAlreadyInstalled);
        ERR_RC_UNLESS(!hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, this->cnt_meta_key.id), err::result::ResultTitleAlreadyInstalled);
        
        bool has_cnmt_installed = false;
        ERR_RC_TRY(ncmContentStorageHas(&this->cnt_storage, &has_cnmt_installed, &record.ContentId));
        if(!has_cnmt_installed) this->ncas.push_back(record);
        this->cnmt.GetInstallContentMeta(this->cnmt_buf, record, global_settings.ignore_required_fw_ver);
        this->base_app_id = hos::GetBaseApplicationId(this->cnt_meta_key.id, static_cast<ncm::ContentMetaType>(this->cnt_meta_key.type));
        this->nacp_data = {};

        auto tik_path = nand_sys_explorer->FullPathFor("Contents/temp/" + tik_file_name);
        if(tik_file_size > 0)
        {
            this->pfs0_file.SaveFile(tik_file_idx, nand_sys_explorer, tik_path);
            this->tik_data = hos::ReadTicket(tik_path);
        }

        for(auto &rec: this->cnmt.GetContentRecords())
        {
            this->ncas.push_back(rec);
            if(rec.Type == ncm::ContentType::Control)
            {
                auto control_nca_content_id = hos::ContentIdAsString(rec.ContentId);
                auto control_nca_file_name = control_nca_content_id + ".nca";
                auto control_nca_file_idx = this->pfs0_file.GetFileIndexByName(control_nca_file_name);
                if(PFS0::IsValidFileIndex(control_nca_file_idx))
                {
                    auto control_nca_temp_path = nand_sys_explorer->FullPathFor("Contents/temp/" + control_nca_file_name);
                    this->pfs0_file.SaveFile(control_nca_file_idx, nand_sys_explorer, control_nca_temp_path);
                    char control_nca_content_path[FS_MAX_PATH] = {0};
                    sprintf(control_nca_content_path, "@SystemContent://temp/%s", control_nca_file_name.AsUTF8().c_str());
                    FsFileSystem control_nca_fs;
                    if(R_SUCCEEDED(fsOpenFileSystemWithId(&control_nca_fs, this->cnt_meta_key.id, FsFileSystemType_ContentControl, control_nca_content_path)))
                    {
                        fs::FspExplorer control_nca_fs_obj("NSP-Control", control_nca_fs);
                        for(auto &cnt: control_nca_fs_obj.GetContents())
                        {
                            if(fs::GetExtension(cnt) == "dat")
                            {
                                this->icon = "sdmc:/" + consts::Root + "/meta/" + control_nca_content_id + ".jpg";
                                control_nca_fs_obj.CopyFile(cnt, this->icon);
                                break;
                            }
                        }
                        control_nca_fs_obj.StartFile("control.nacp", fs::FileMode::Read);
                        control_nca_fs_obj.ReadFileBlock("control.nacp", 0, sizeof(nacp_data), &nacp_data);
                        control_nca_fs_obj.EndFile(fs::FileMode::Read);
                    }
                }
            }
        }
        return err::result::ResultSuccess;
    }

    Result Installer::PreProcessContents()
    {
        ERR_RC_TRY(ncmContentMetaDatabaseSet(&this->cnt_meta_db, &this->cnt_meta_key, this->cnmt_buf.GetData(), this->cnmt_buf.GetSize()));
        ERR_RC_TRY(ncmContentMetaDatabaseCommit(&this->cnt_meta_db));
        s32 content_meta_count = 0;
        auto rc = nsCountApplicationContentMeta(this->base_app_id, &content_meta_count);
        if(rc == 0x410) rc = err::result::ResultSuccess;
        ERR_RC_TRY(rc);
        std::vector<ns::ContentStorageRecord> content_storage_records;
        if(content_meta_count > 0)
        {
            auto tmp_buf = reinterpret_cast<ns::ContentStorageRecord*>(fs::GetWorkBuffer());
            u32 real_count = 0;
            ERR_RC_TRY(ns::ListApplicationRecordContentMeta(0, this->base_app_id, tmp_buf, content_meta_count * sizeof(ns::ContentStorageRecord), &real_count));
            for(u32 i = 0; i < real_count; i++) content_storage_records.push_back(tmp_buf[i]);
        }
        ns::ContentStorageRecord content_storage_record = {};
        content_storage_record.Record = this->cnt_meta_key;
        content_storage_record.StorageId = this->storage_id;
        content_storage_records.push_back(content_storage_record);
        ns::DeleteApplicationRecord(this->base_app_id);
        ERR_RC_TRY(ns::PushApplicationRecord(this->base_app_id, 3, content_storage_records.data(), content_storage_records.size() * sizeof(ns::ContentStorageRecord)));
        if(this->tik_file_size > 0)
        {
            auto tmp_buf = fs::GetWorkBuffer();
            auto tik_path = "Contents/temp/" + this->tik_file_name;
            auto nand_sys_explorer = fs::GetNANDSystemExplorer();
            nand_sys_explorer->StartFile(tik_path, fs::FileMode::Read);
            nand_sys_explorer->ReadFileBlock(tik_path, 0, this->tik_file_size, tmp_buf);
            nand_sys_explorer->EndFile(fs::FileMode::Read);
            ERR_RC_TRY(es::ImportTicket(tmp_buf, this->tik_file_size, es::CertData, es::CertSize));
        }
        return err::result::ResultSuccess;
    }

    ncm::ContentMetaType Installer::GetContentMetaType()
    {
        return static_cast<ncm::ContentMetaType>(this->cnt_meta_key.type);
    }

    u64 Installer::GetApplicationId()
    {
        return this->cnt_meta_key.id;
    }

    std::string Installer::GetExportedIconPath()
    {
        return this->icon.AsUTF8();
    }

    NacpStruct *Installer::GetNACP()
    {
        return &this->nacp_data;
    }

    bool Installer::HasTicket()
    {
        return this->tik_file_size > 0;
    }

    hos::TicketData Installer::GetTicketData()
    {
        return this->tik_data;
    }

    u8 Installer::GetKeyGeneration()
    {
        return this->keygen;
    }

    std::vector<ncm::ContentRecord> Installer::GetNCAs()
    {
        return this->ncas;
    }

    Result Installer::WriteContents(OnContentsWriteFunction OnContentWrite)
    {
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        auto read_size = fs::WorkBufferSize;
        auto tmp_buf = fs::GetWorkBuffer();
        u64 total_size = 0;
        u64 total_written_size = 0;
        std::vector<u32> content_file_idxs;
        for(auto &cnt: this->ncas)
        {
            auto cnt_id = cnt.ContentId;
            auto content_file_name = hos::ContentIdAsString(cnt_id);
            if(cnt.Type == ncm::ContentType::Meta) content_file_name += ".cnmt";
            content_file_name += ".nca";
            auto content_file_idx = this->pfs0_file.GetFileIndexByName(content_file_name);
            ERR_RC_UNLESS(PFS0::IsValidFileIndex(content_file_idx), err::result::ResultInvalidNSP);
            total_size += pfs0_file.GetFileSize(content_file_idx);
            content_file_idxs.push_back(content_file_idx);
        }
        for(u32 i = 0; i < this->ncas.size(); i++)
        {
            auto cnt = this->ncas[i];
            auto cnt_id = cnt.ContentId;
            auto content_file_idx = content_file_idxs[i];
            auto content_file_name = this->pfs0_file.GetFile(content_file_idx);
            auto content_file_size = this->pfs0_file.GetFileSize(content_file_idx);

            NcmPlaceHolderId placehld_id = {};
            memcpy(placehld_id.uuid.uuid, cnt_id.c, 0x10);
            
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &placehld_id);
            ERR_RC_TRY(ncmContentStorageCreatePlaceHolder(&this->cnt_storage, &cnt_id, &placehld_id, content_file_size));
            u64 cur_written_size = 0;
            u64 rem_size = content_file_size;
            auto content_path = "Contents/temp/" + content_file_name;
            switch(cnt.Type)
            {
                case ncm::ContentType::Meta:
                case ncm::ContentType::Control:
                    nand_sys_explorer->StartFile(content_path, fs::FileMode::Read);
                    break;
                default:
                    pfs0_file.GetExplorer()->StartFile(pfs0_file.GetPath(), fs::FileMode::Read);
                    break;
            }
            while(rem_size)
            {
                u64 tmp_read_size = 0;
                auto proper_size = std::min(rem_size, read_size);
                auto time_pre = std::chrono::steady_clock::now();
                switch(cnt.Type)
                {
                    case ncm::ContentType::Meta:
                    case ncm::ContentType::Control:
                        tmp_read_size = nand_sys_explorer->ReadFileBlock(content_path, cur_written_size, proper_size, tmp_buf);
                        break;
                    default:
                        tmp_read_size = pfs0_file.ReadFromFile(content_file_idx, cur_written_size, proper_size, tmp_buf);
                        break;
                }
                ERR_RC_TRY(ncmContentStorageWritePlaceHolder(&this->cnt_storage, &placehld_id, cur_written_size, tmp_buf, tmp_read_size));
                cur_written_size += tmp_read_size;
                rem_size -= tmp_read_size;
                auto time_post = std::chrono::steady_clock::now();
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_post - time_pre).count();
                auto bytes_per_sec = (1000.0f / (double)(diff)) * (double)(tmp_read_size); // By elapsed time and written bytes, compute how much data has been written in 1 second
                OnContentWrite(cnt, i, this->ncas.size(), (double)(cur_written_size + total_written_size), (double)total_size, (u64)bytes_per_sec);
            }
            switch(cnt.Type)
            {
                case ncm::ContentType::Meta:
                case ncm::ContentType::Control:
                    nand_sys_explorer->EndFile(fs::FileMode::Read);
                    break;
                default:
                    pfs0_file.GetExplorer()->EndFile(fs::FileMode::Read);
                    break;
            }
            total_written_size += cur_written_size;
            ERR_RC_TRY(ncmContentStorageRegister(&this->cnt_storage, &cnt_id, &placehld_id));
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &placehld_id);
        }
        return err::result::ResultSuccess;
    }

    void Installer::FinalizeInstallation()
    {
        ncmContentStorageClose(&this->cnt_storage);
        ncmContentMetaDatabaseClose(&this->cnt_meta_db);
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        nand_sys_explorer->DeleteDirectory("Contents/temp");
    }
}