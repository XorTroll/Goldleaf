
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

#include <nsp/nsp_Installer.hpp>
#include <fs/fs_FileSystem.hpp>
#include <hos/hos_Common.hpp>
#include <cfg/cfg_Settings.hpp>

extern cfg::Settings g_Settings;

namespace nsp {

    namespace {

        struct ContentWriteBuffer {
            u32 cnt_id;
            u8 *buf;
            size_t size;

            ContentWriteBuffer() {}
            
            ContentWriteBuffer(const u32 cnt_id, u8 *buf, const size_t size) {
                this->cnt_id = cnt_id;
                this->buf = buf;
                this->size = size;
            }
        };

        class ContentWriteContext {
            private:
                OnContentWriteFunction on_content_write_fn;
                NcmContentStorage cnt_storage;
                std::queue<ContentWriteBuffer> buffer_queue;
                Lock buffer_queue_lock;
                ContentWriteProgress write_progress;
                Lock write_progress_lock;
                Result last_rc;
                Lock last_rc_lock;
                std::atomic_bool done;

            public:
                enum class Status {
                    Done,
                    BufferQueueAvailable,
                    BufferQueueEmpty
                };

                ContentWriteContext(OnContentWriteFunction on_content_write_fn, NcmContentStorage cnt_storage) : on_content_write_fn(on_content_write_fn), cnt_storage(cnt_storage), buffer_queue(), buffer_queue_lock(), write_progress(), write_progress_lock(), last_rc(rc::ResultSuccess), last_rc_lock(), done(false) {}

                void EmplaceBuffer(const u32 cnt_id, u8 *buf, const size_t size) {
                    ScopedLock queue_lock(this->buffer_queue_lock);
                    this->buffer_queue.emplace(cnt_id, buf, size);
                }

                Status GetStatus() {
                    ScopedLock queue_lock(this->buffer_queue_lock);
                    if(this->buffer_queue.empty()) {
                        if(this->done) {
                            return Status::Done;
                        }
                        else {
                            return Status::BufferQueueEmpty;
                        }
                    }
                    else {
                        return Status::BufferQueueAvailable;
                    }
                }

                u32 RegisterContent(const NcmContentType type, const NcmPlaceHolderId placehld_id, const size_t total_size) {
                    ScopedLock progress_lock(this->write_progress_lock);

                    const auto cnt_id = this->write_progress.entries.size();
                    this->write_progress.entries.push_back({
                        .type = type,
                        .placehld_id = placehld_id,
                        .cur_offset = 0,
                        .size = total_size
                    });
                    return cnt_id;
                }

                Result PopHandleNextBuffer() {
                    // Ensure two buffers are not written at the same time
                    // TODO: multi-thread/multiple NCM instances...?
                    ScopedLock queue_lock(this->buffer_queue_lock);
                    const auto buf = this->buffer_queue.front();
                    this->buffer_queue.pop();

                    NcmPlaceHolderId placehld_id;
                    size_t offset;
                    {
                        ScopedLock progress_lock(this->write_progress_lock);
                        auto &cnt_entry = this->write_progress.entries.at(buf.cnt_id);
                        placehld_id = cnt_entry.placehld_id;
                        offset = cnt_entry.cur_offset;
                        cnt_entry.cur_offset += buf.size;
                        this->write_progress.written_size += buf.size;
                    }
                    const auto rc = ncmContentStorageWritePlaceHolder(&this->cnt_storage, &placehld_id, offset, buf.buf, buf.size);
                    fs::DeleteWorkBuffer(buf.buf);

                    {
                        ScopedLock rc_lock(this->last_rc_lock);
                        this->last_rc = rc;
                    }
                    return rc;
                }

                void SignalDone() {
                    this->done = true;
                }

                Result GetLastResult() {
                    ScopedLock rc_lock(this->last_rc_lock);
                    return this->last_rc;
                }

                void NotifyUpdateProgress() {
                    ScopedLock progress_lock(this->write_progress_lock);
                    this->on_content_write_fn(this->write_progress);
                    this->write_progress.written_size = 0;
                }

                void NotifyStart(OnStartWriteFunction on_start_write_fn) {
                    on_start_write_fn(this->write_progress);
                }
        };

        void ContentWriteThread(void *ctx_raw) {
            auto ctx = reinterpret_cast<ContentWriteContext*>(ctx_raw);

            while(true) {
                const auto status = ctx->GetStatus();
                if(status == ContentWriteContext::Status::Done) {
                    break;
                }
                else if(status == ContentWriteContext::Status::BufferQueueAvailable) {
                    const auto rc = ctx->PopHandleNextBuffer();
                    if(R_FAILED(rc)) {
                        ctx->SignalDone();
                        break;
                    }
                }

                svcSleepThread(100'000ul);
            }
        }

    }

    Installer::~Installer() {
        FinalizeInstallation();
    }

    Result Installer::PrepareInstallation() {
        GLEAF_RC_UNLESS(pfs0_file.IsOk(), rc::goldleaf::ResultInvalidNsp);
        GLEAF_RC_TRY(ncmOpenContentStorage(&this->cnt_storage, this->storage_id));
        GLEAF_RC_TRY(ncmOpenContentMetaDatabase(&this->cnt_meta_db, this->storage_id));

        std::string cnmt_nca_file_name;
        auto cnmt_nca_file_idx = fs::PFS0::InvalidFileIndex;
        u64 cnmt_nca_file_size = 0;
        auto tik_file_idx = fs::PFS0::InvalidFileIndex;
        tik_file_size = 0;
        const auto pfs0_files = pfs0_file.GetFiles();
        for(u32 i = 0; i < pfs0_files.size(); i++) {
            const auto file = pfs0_files.at(i);
            if(fs::GetExtension(file) == "tik") {
                tik_file_name = file;
                tik_file_idx = i;
                tik_file_size = pfs0_file.GetFileSize(i);
            }
            else {
                if(file.length() >= __builtin_strlen("cnmt.nca")) {
                    if(file.substr(file.length() - __builtin_strlen("cnmt.nca")) == "cnmt.nca") {
                        cnmt_nca_file_name = file;
                        cnmt_nca_file_idx = i;
                        cnmt_nca_file_size = pfs0_file.GetFileSize(i);
                    }
                }
            }
        }
        GLEAF_RC_UNLESS(fs::PFS0::IsValidFileIndex(cnmt_nca_file_idx), rc::goldleaf::ResultMetaNotFound);
        GLEAF_RC_UNLESS(cnmt_nca_file_size > 0, rc::goldleaf::ResultMetaNotFound);
        const auto cnmt_nca_content_id = fs::GetBaseName(cnmt_nca_file_name);

        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        nand_sys_explorer->CreateDirectory("Contents/temp");

        if(tik_file_size > 0) {
            auto tik_path = nand_sys_explorer->FullPathFor("Contents/temp/" + tik_file_name);
            this->pfs0_file.SaveFile(tik_file_idx, nand_sys_explorer, tik_path);
            this->tik_file = hos::ReadTicket(tik_path);
        }

        const auto cnmt_nca_temp_path = nand_sys_explorer->FullPathFor("Contents/temp/" + cnmt_nca_file_name);
        nand_sys_explorer->DeleteFile(cnmt_nca_temp_path);
        pfs0_file.SaveFile(cnmt_nca_file_idx, nand_sys_explorer, cnmt_nca_temp_path);

        char cnmt_nca_content_path[FS_MAX_PATH] = {};
        snprintf(cnmt_nca_content_path, sizeof(cnmt_nca_content_path), "@SystemContent://temp/%s", cnmt_nca_file_name.c_str());
        FsRightsId tmp_rid;
        GLEAF_RC_TRY(fsGetRightsIdAndKeyGenerationByPath(cnmt_nca_content_path, FsContentAttributes_All, &this->keygen, &tmp_rid));
        const auto system_keygen = hos::ReadSystemKeyGeneration();
        GLEAF_RC_UNLESS(system_keygen >= this->keygen, rc::goldleaf::ResultKeyGenerationMismatch);

        FsFileSystem cnmt_nca_fs;
        GLEAF_RC_TRY(fsOpenFileSystemWithId(&cnmt_nca_fs, 0, FsFileSystemType_ContentMeta, cnmt_nca_content_path, FsContentAttributes_All));
        
        {
            fs::FspExplorer cnmt_nca_fs_obj(cnmt_nca_fs, "nsp.ContentMeta");
            std::string cnmt_file_name;
            for(const auto &cnt: cnmt_nca_fs_obj.GetContents()) {
                if(fs::GetExtension(cnt) == "cnmt") {
                    cnmt_file_name = cnt;
                    break;
                }
            }
            GLEAF_RC_UNLESS(!cnmt_file_name.empty(), rc::goldleaf::ResultCnmtNotFound);

            const auto cnmt_file_size = cnmt_nca_fs_obj.GetFileSize(cnmt_file_name);
            auto cnmt_read_buf = fs::AllocateWorkBuffer(cnmt_file_size);
            ScopeGuard on_exit([&]() {
                fs::DeleteWorkBuffer(cnmt_read_buf);
            });

            cnmt_nca_fs_obj.ReadFile(cnmt_file_name, 0, cnmt_file_size, cnmt_read_buf);
            GLEAF_RC_UNLESS(ncm::ReadContentMeta(cnmt_read_buf, cnmt_file_size, this->packaged_cnt_meta), rc::goldleaf::ResultInvalidMeta);
        }

        this->meta_cnt_info = {
            .content_id = hos::StringAsContentId(cnmt_nca_content_id),
            .content_type = NcmContentType_Meta,
        };
        ncmU64ToContentInfoSize(cnmt_nca_file_size, &this->meta_cnt_info);

        this->programs.clear();
        for(const auto &cnt: this->packaged_cnt_meta.contents) {
            if(cnt.info.content_type == NcmContentType_Program) {
                const auto program_idx = this->programs.size();

                auto &program = this->programs.emplace_back();
                program.meta_key = {
                    .id = this->packaged_cnt_meta.header.app_id + program_idx,
                    .version = this->packaged_cnt_meta.header.version,
                    .type = this->packaged_cnt_meta.header.type,
                    .install_type = NcmContentInstallType_Full
                };
            }
        }
        if(this->programs.empty()) {
            auto &main_program = this->programs.emplace_back();
            main_program.meta_key = {
                .id = this->packaged_cnt_meta.header.app_id,
                .version = this->packaged_cnt_meta.header.version,
                .type = this->packaged_cnt_meta.header.type,
                .install_type = NcmContentInstallType_Full
            };
        }

        GLEAF_RC_UNLESS(!hos::ExistsTitle(NcmStorageId_SdCard, NcmContentMetaType_Unknown, this->programs.front().meta_key.id), rc::goldleaf::ResultTitleAlreadyInstalled);
        GLEAF_RC_UNLESS(!hos::ExistsTitle(NcmStorageId_BuiltInUser, NcmContentMetaType_Unknown, this->programs.front().meta_key.id), rc::goldleaf::ResultTitleAlreadyInstalled);
        
        bool has_cnmt_installed = false;
        GLEAF_RC_TRY(ncmContentStorageHas(&this->cnt_storage, &has_cnmt_installed, &this->meta_cnt_info.content_id));
        if(!has_cnmt_installed) {
            this->contents.push_back(this->meta_cnt_info);
        }

        // TODO: improve? this supposes that programs are ordered according to their ids
        u32 program_i = 0;
        for(const auto &cnt: this->packaged_cnt_meta.contents) {
            this->contents.push_back(cnt.info);
            if(cnt.info.content_type == NcmContentType_Control) {
                auto &cur_program = this->programs.at(program_i);
                const auto control_nca_content_id = hos::ContentIdAsString(cnt.info.content_id);
                const auto control_nca_file_name = control_nca_content_id + ".nca";
                const auto control_nca_file_idx = this->pfs0_file.GetFileIndexByName(control_nca_file_name);
                if(fs::PFS0::IsValidFileIndex(control_nca_file_idx)) {
                    const auto control_nca_temp_path = nand_sys_explorer->MakeFull("Contents/temp/" + control_nca_file_name);
                    this->pfs0_file.SaveFile(control_nca_file_idx, nand_sys_explorer, control_nca_temp_path);

                    char control_nca_content_path[FS_MAX_PATH] = {};
                    snprintf(control_nca_content_path, sizeof(control_nca_content_path), "@SystemContent://temp/%s", control_nca_file_name.c_str());

                    FsFileSystem control_nca_fs;
                    if(R_SUCCEEDED(fsOpenFileSystemWithId(&control_nca_fs, cur_program.meta_key.id, FsFileSystemType_ContentControl, control_nca_content_path, FsContentAttributes_All))) {
                        fs::FspExplorer control_nca_fs_obj(control_nca_fs, "nsp.ControlData");
                        
                        const auto cur_lang_name = GetLanguageName(g_Settings.GetLanguage());
                        std::string icon_dat_cnt;
                        for(const auto &cnt: control_nca_fs_obj.GetContents()) {
                            if(fs::GetExtension(cnt) == "dat") {
                                if(icon_dat_cnt.empty()) {
                                    // Use the first icon if the correct one isn't found
                                    icon_dat_cnt = cnt;
                                }

                                // Remove the "icon_" prefix
                                const auto icon_dat_lang = fs::GetFileName(cnt).substr(__builtin_strlen("icon_"));
                                if(cur_lang_name == icon_dat_lang) {
                                    icon_dat_cnt = cnt;
                                    break;
                                }
                            }
                        }

                        auto sd_exp = fs::GetSdCardExplorer();
                        cur_program.icon_path = sd_exp->MakeAbsolute(GLEAF_PATH_METADATA_DIR "/" + control_nca_content_id + ".jpg");
                        control_nca_fs_obj.CopyFile(icon_dat_cnt, cur_program.icon_path);
                        control_nca_fs_obj.ReadFile("control.nacp", 0, sizeof(cur_program.nacp_data), &cur_program.nacp_data);
                    }
                }
                program_i++;
            }
        }
        GLEAF_RC_SUCCEED;
    }

    Result Installer::StartInstallation() {
        const auto &main_program = this->programs.front();
        const auto base_app_id = hos::GetBaseApplicationId(main_program.meta_key.id, static_cast<NcmContentMetaType>(main_program.meta_key.type));

        u8 *meta_data;
        size_t meta_data_size;
        this->packaged_cnt_meta.CreateContentMetaForInstall(this->meta_cnt_info, meta_data, meta_data_size, g_Settings.ignore_required_fw_ver);
        GLEAF_RC_TRY(ncmContentMetaDatabaseSet(&this->cnt_meta_db, std::addressof(main_program.meta_key), meta_data, meta_data_size));
        GLEAF_RC_TRY(ncmContentMetaDatabaseCommit(&this->cnt_meta_db));
        delete[] meta_data;

        s32 content_meta_count = 0;
        GLEAF_RC_TRY_EXCEPT(nsCountApplicationContentMeta(base_app_id, &content_meta_count), 0x410);

        std::vector<ns::ContentStorageMetaKey> content_storage_meta_keys;
        if(content_meta_count > 0) {
            auto cnt_storage_meta_key_buf = new ns::ContentStorageMetaKey[content_meta_count]();
            u32 real_count = 0;
            GLEAF_RC_TRY(ns::ListApplicationRecordContentMeta(0, base_app_id, cnt_storage_meta_key_buf, content_meta_count, &real_count));
            content_storage_meta_keys.reserve(real_count);
            for(u32 i = 0; i < real_count; i++) {
                content_storage_meta_keys.push_back(cnt_storage_meta_key_buf[i]);
            }
            delete[] cnt_storage_meta_key_buf;
        }

        const ns::ContentStorageMetaKey cnt_storage_meta_key = {
            .meta_key = main_program.meta_key,
            .storage_id = this->storage_id,
        };
        content_storage_meta_keys.push_back(cnt_storage_meta_key);
        ns::DeleteApplicationRecord(base_app_id);
        GLEAF_RC_TRY(ns::PushApplicationRecord(base_app_id, 3, content_storage_meta_keys.data(), content_storage_meta_keys.size()));

        if(this->tik_file_size > 0) {
            auto tik_buf = fs::AllocateWorkBuffer(this->tik_file_size);
            ScopeGuard on_exit([&]() {
                fs::DeleteWorkBuffer(tik_buf);
            });

            const auto tik_path = "Contents/temp/" + this->tik_file_name;
            auto nand_sys_explorer = fs::GetNANDSystemExplorer();
            nand_sys_explorer->ReadFile(tik_path, 0, this->tik_file.GetFullSize(), tik_buf);
            GLEAF_RC_TRY(es::ImportTicket(tik_buf, this->tik_file_size, es::CommonCertificateData, es::CommonCertificateSize));
        }
        GLEAF_RC_SUCCEED;
    }

    Result Installer::WriteContents(OnStartWriteFunction on_start_write_fn, OnContentWriteFunction on_content_write_fn) {
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        u64 total_size = 0;
        u64 total_written_size = 0;
        std::vector<u32> content_file_idxs;
        std::vector<NcmPlaceHolderId> content_placehld_ids;
        std::vector<u32> content_write_idxs;
        for(const auto &cnt: this->contents) {
            const auto content_file_name = hos::ContentIdAsString(cnt.content_id) + ((cnt.content_type == NcmContentType_Meta) ? ".cnmt" : "") + ".nca";
            const auto content_file_idx = this->pfs0_file.GetFileIndexByName(content_file_name);
            GLEAF_RC_UNLESS(fs::PFS0::IsValidFileIndex(content_file_idx), rc::goldleaf::ResultInvalidNsp);
            total_size += pfs0_file.GetFileSize(content_file_idx);
            content_file_idxs.push_back(content_file_idx);
        }

        ContentWriteContext write_ctx(on_content_write_fn, this->cnt_storage);
        for(u32 i = 0; i < this->contents.size(); i++) {
            const auto &cnt = this->contents.at(i);
            const auto content_file_idx = content_file_idxs.at(i);
            const auto content_file_size = this->pfs0_file.GetFileSize(content_file_idx);

            NcmPlaceHolderId placehld_id = {};
            memcpy(placehld_id.uuid.uuid, cnt.content_id.c, sizeof(placehld_id.uuid.uuid));
            content_placehld_ids.push_back(placehld_id);
            const auto cnt_id = write_ctx.RegisterContent(static_cast<NcmContentType>(cnt.content_type), placehld_id, content_file_size);
            content_write_idxs.push_back(cnt_id);
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &placehld_id);
            GLEAF_RC_TRY(ncmContentStorageCreatePlaceHolder(&this->cnt_storage, &cnt.content_id, &placehld_id, content_file_size));
        }

        write_ctx.NotifyStart(on_start_write_fn);

        Thread cnt_write_thread;
        GLEAF_RC_TRY(threadCreate(&cnt_write_thread, ContentWriteThread, reinterpret_cast<void*>(&write_ctx), nullptr, 0x100000, 0x3B, -2));
        GLEAF_RC_TRY(threadStart(&cnt_write_thread));

        for(u32 i = 0; i < this->contents.size(); i++) {
            const auto &cnt = this->contents.at(i);
            const auto content_file_idx = content_file_idxs.at(i);
            const auto content_file_name = this->pfs0_file.GetFile(content_file_idx);
            const auto content_file_size = this->pfs0_file.GetFileSize(content_file_idx);
            const auto content_write_id = content_write_idxs.at(i);
            
            u64 cur_written_size = 0;
            auto rem_size = content_file_size;
            const auto content_path = "Contents/temp/" + content_file_name;
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
                const auto last_rc = write_ctx.GetLastResult();
                if(R_FAILED(last_rc)) {
                    GLEAF_RC_TRY(threadWaitForExit(&cnt_write_thread));
                    GLEAF_RC_TRY(threadClose(&cnt_write_thread));
                    return last_rc;
                }

                const auto read_size = std::min(rem_size, g_Settings.copy_buffer_max_size);
                auto read_buf = fs::AllocateWorkBuffer(read_size);
                u64 tmp_read_size = 0;
                switch(cnt.content_type) {
                    case NcmContentType_Meta:
                    case NcmContentType_Control: {
                        tmp_read_size = nand_sys_explorer->ReadFile(content_path, cur_written_size, read_size, read_buf);
                        break;
                    }
                    default: {
                        tmp_read_size = pfs0_file.ReadFromFile(content_file_idx, cur_written_size, read_size, read_buf);
                        break;
                    }
                }
                write_ctx.EmplaceBuffer(content_write_id, read_buf, read_size);

                cur_written_size += tmp_read_size;
                rem_size -= tmp_read_size;

                write_ctx.NotifyUpdateProgress();
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
        }

        write_ctx.SignalDone();
        GLEAF_RC_TRY(threadWaitForExit(&cnt_write_thread));
        GLEAF_RC_TRY(threadClose(&cnt_write_thread));
        write_ctx.NotifyUpdateProgress();

        for(u32 i = 0; i < this->contents.size(); i++) {
            const auto &cnt = this->contents.at(i);
            const auto content_placehld_id = content_placehld_ids.at(i);

            GLEAF_RC_TRY(ncmContentStorageRegister(&this->cnt_storage, &cnt.content_id, &content_placehld_id));
            ncmContentStorageDeletePlaceHolder(&this->cnt_storage, &content_placehld_id);
        }

        GLEAF_RC_SUCCEED;
    }

    void Installer::FinalizeInstallation() {
        ncmContentStorageClose(&this->cnt_storage);
        ncmContentMetaDatabaseClose(&this->cnt_meta_db);
        auto nand_sys_explorer = fs::GetNANDSystemExplorer();
        nand_sys_explorer->DeleteDirectory("Contents/temp");
    }

}