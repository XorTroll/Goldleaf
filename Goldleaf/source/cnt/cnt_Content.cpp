
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

#include <cnt/cnt_Content.hpp>
#include <cnt/cnt_Names.hpp>
#include <util/util_String.hpp>
#include <fs/fs_FileSystem.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;

namespace cnt {

    namespace {

        inline ApplicationPlayStats ConvertPlayStats(const PdmPlayStatistics play_stats) {
            const auto posix_time = time(nullptr);

            return {
                .total_play_secs = (u64)(((double)play_stats.playtime) / 1.0e9),
                .secs_from_last_launched = posix_time - play_stats.last_timestamp_user,
                .secs_from_first_launched = posix_time - play_stats.first_timestamp_user
            };
        }

        std::vector<Application> g_Applications;
        Lock g_ApplicationsLock;

        constexpr size_t ApplicationRecordBufferCount = 30;
        NsExtApplicationRecord g_ApplicationRecordBuffer[ApplicationRecordBufferCount];

        constexpr size_t ApplicationContentMetaStatusBufferCount = 100;
        NsApplicationContentMetaStatus g_ApplicationContentMetaStatusBuffer[ApplicationContentMetaStatusBufferCount];

        NsApplicationControlData g_TemporaryApplicationControlData;

        inline Result GetApplicationControlData(const u64 app_id, NsApplicationControlData &out_data, size_t &out_icon_size) {
            size_t got_size;
            Result rc;
            if(hosversionAtLeast(20,0,0)) {
                rc = nsGetApplicationControlData2(NsApplicationControlSource_Storage, app_id, std::addressof(out_data), sizeof(out_data), 0xFF, 0, &got_size, nullptr);
            }
            else {
                rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, app_id, std::addressof(out_data), sizeof(out_data), &got_size);
            }
            if(R_SUCCEEDED(rc)) {
                GLEAF_ASSERT_TRUE(got_size <= sizeof(out_data));
                out_icon_size = got_size - sizeof(NacpStruct);
            }
            return rc;
        }

        void ScanApplicationRecords() {
            s32 cur_offset = 0;
            while(true) {
                s32 record_count = 0;
                if(R_FAILED(nsListApplicationRecord(reinterpret_cast<NsApplicationRecord*>(g_ApplicationRecordBuffer), ApplicationRecordBufferCount, cur_offset, &record_count))) {
                    break;
                }
                if(record_count == 0) {
                    break;
                }

                cur_offset += record_count;
                for(s32 i = 0; i < record_count; i++) {
                    const auto app_record = g_ApplicationRecordBuffer[i];

                    auto app_it = std::find_if(g_Applications.begin(), g_Applications.end(), [&](const Application &app) -> bool {
                        return app.record.id == app_record.id;
                    });
                    Application *app_ref;
                    if(app_it != g_Applications.end()) {
                        app_it->record = app_record;
                        app_ref = std::addressof(*app_it);
                    }
                    else {
                        auto &app = g_Applications.emplace_back();
                        app.record = app_record;
                        app_ref = std::addressof(app);
                    }
                    app_ref->cache.record_last_event = GetApplicationEventName(static_cast<NsExtApplicationEvent>(app_record.last_event));

                    s32 status_count = 0;
                    nsListApplicationContentMetaStatus(app_record.id, 0, g_ApplicationContentMetaStatusBuffer, ApplicationContentMetaStatusBufferCount, &status_count);

                    for(s32 j = 0; j < status_count; j++) {
                        auto &content = app_ref->contents.emplace_back();

                        const auto &status = g_ApplicationContentMetaStatusBuffer[j];
                        NcmContentMetaDatabase meta_db;
                        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&meta_db, static_cast<NcmStorageId>(status.storageID)))) {
                            if(R_SUCCEEDED(ncmContentMetaDatabaseGetLatestContentMetaKey(&meta_db, &content.meta_key, status.application_id))) {
                                for(u32 i = 0; i < MaxContentCount; i++) {
                                    NcmContentId cnt_id;
                                    if(R_SUCCEEDED(ncmContentMetaDatabaseGetContentIdByType(&meta_db, &cnt_id, &content.meta_key, static_cast<NcmContentType>(i)))) {
                                        content.cnt_ids[i] = cnt_id;
                                    }
                                    else {
                                        content.cnt_ids[i] = {};
                                    }
                                }
                            }

                            ncmContentMetaDatabaseClose(&meta_db);
                        }
                    }

                    if(status_count > 0) {
                        app_ref->meta_status_list = std::vector<NsApplicationContentMetaStatus>(g_ApplicationContentMetaStatusBuffer, g_ApplicationContentMetaStatusBuffer + status_count);
                    }
                }
            }
        }

        bool SortApplicationsImpl(cnt::Application &app_a, cnt::Application &app_b) {
            return app_a.cache.display_name.front() < app_b.cache.display_name.front();
        }

        Thread g_LoadApplicationsThread;
        std::atomic_bool g_LoadApplicationsThreadShouldExit = false;
        std::atomic_bool g_LoadApplicationsThreadDone = true;

        void ScanApplications() {
            ScopedLock lk(g_ApplicationsLock);
            g_Applications.clear();

            // Scan application records and meta status
            ScanApplicationRecords();

            // Try to get application views and control data
            for(auto &app: g_Applications) {
                if(g_LoadApplicationsThreadShouldExit) {
                    g_LoadApplicationsThreadShouldExit = false;
                    return;
                }

                if(R_SUCCEEDED(nsGetApplicationView(reinterpret_cast<NsApplicationView*>(&app.view), &app.record.id, 1))) {
                    for(u32 i = 0; i < sizeof(app.view.flags) * CHAR_BIT; i++) {
                        if(app.view.flags & BIT(i)) {
                            app.cache.view_flags.push_back(GetApplicationViewFlagName(i));
                        }
                    }
                }

                app.cache.display_name = util::FormatApplicationId(app.record.id);
                app.cache.display_author = "";
                size_t dummy;
                if(R_SUCCEEDED(GetApplicationControlData(app.record.id, g_TemporaryApplicationControlData, dummy))) {
                    memcpy(app.misc_data.display_version, g_TemporaryApplicationControlData.nacp.display_version, sizeof(g_TemporaryApplicationControlData.nacp.display_version));
                    app.misc_data.device_save_data_size = g_TemporaryApplicationControlData.nacp.device_save_data_size;
                    app.misc_data.user_account_save_data_size = g_TemporaryApplicationControlData.nacp.user_account_save_data_size;

                    NacpLanguageEntry *lang_entry = nullptr;
                    if(R_SUCCEEDED(nacpGetLanguageEntry(&g_TemporaryApplicationControlData.nacp, &lang_entry)) && lang_entry != nullptr) {
                        if(lang_entry->name[0] != '\0') {
                            app.cache.display_name = std::string(lang_entry->name);
                        }
                        if(lang_entry->author[0] != '\0') {
                            app.cache.display_author = std::string(lang_entry->author);
                        }
                    }
                }
                else {
                    strcpy(app.misc_data.display_version, "<unknown>");
                    app.misc_data.device_save_data_size = 0;
                    app.misc_data.user_account_save_data_size = 0;
                }

                nsCalculateApplicationOccupiedSize(app.record.id, reinterpret_cast<NsApplicationOccupiedSize*>(&app.occupied_size));

                GLEAF_LOG_FMT("Application %016lX:", app.record.id);

                GLEAF_LOG_FMT("  - NACP title: %s", app.cache.display_name.c_str());
                GLEAF_LOG_FMT("  - NACP author: %s", app.cache.display_author.c_str());
                GLEAF_LOG_FMT("  - NACP version: %s", app.misc_data.display_version);

                if(app.record.id != 0) {
                    GLEAF_LOG_FMT("  - Record last event: %s", app.cache.record_last_event.c_str());
                }
                else {
                    GLEAF_LOG_FMT("  ! <no record>");
                }

                if(app.view.app_id != 0) {
                    GLEAF_LOG_FMT("  - View flags: %s", util::JoinVector(app.cache.view_flags, ", ").c_str());
                }
                else {
                    GLEAF_LOG_FMT("  ! <no view>");
                }

                if(app.meta_status_list.size() > 0) {
                    GLEAF_LOG_FMT("  - Meta status count: %ld", app.meta_status_list.size());
                    for(u32 i = 0; i < app.meta_status_list.size(); i++) {
                        const auto &status = app.meta_status_list[i];
                        const auto &content = app.contents[i];
                        GLEAF_LOG_FMT("     [%d] Status application ID: %016lX", i, status.application_id);
                        GLEAF_LOG_FMT("     [%d] Status meta type: %d", i, status.meta_type);
                        GLEAF_LOG_FMT("     [%d] Status version: %d", i, status.version);
                        for(u32 j = 0; j < MaxContentCount; j++) {
                            if(content.cnt_ids[j].has_value()) {
                                GLEAF_LOG_FMT("          [%d] Content ID: '%s'", j, util::FormatContentId(content.cnt_ids[j].value()).c_str());
                            }
                            else {
                                GLEAF_LOG_FMT("          [%d] Content ID: <none>", j);
                            }
                        }
                        i++;
                    }
                }
                else {
                    GLEAF_LOG_FMT("  ! <no meta status>");
                }
            }

            std::sort(g_Applications.begin(), g_Applications.end(), SortApplicationsImpl);
        }

        void LoadApplicationsMain(void*) {
            SetThreadName("cnt.LoadApplicationsThread");

            GLEAF_LOG_FMT("Scanning applications...");
            ScanApplications();

            GLEAF_LOG_FMT("Done! Exiting thread...");
            g_LoadApplicationsThreadDone = true;
        }

        void RequestLoadApplications() {
            if(!g_LoadApplicationsThreadDone) {
                threadWaitForExit(&g_LoadApplicationsThread);
            }
            threadClose(&g_LoadApplicationsThread);

            g_LoadApplicationsThreadDone = false;
            GLEAF_RC_ASSERT(threadCreate(&g_LoadApplicationsThread, LoadApplicationsMain, nullptr, nullptr, 512_KB, 0x1F, -2));
            GLEAF_RC_ASSERT(threadStart(&g_LoadApplicationsThread));

            svcSleepThread(10'000'000ul);
        }

        Result RemoveApplicationContentMeta(const cnt::Application &app, const cnt::ApplicationContent &cnt) {
            s32 content_meta_count;
            GLEAF_RC_TRY(nsCountApplicationContentMeta(app.record.id, &content_meta_count));
    
            std::vector<NsExtContentStorageMetaKey> content_storage_meta_keys;
            if(content_meta_count > 0) {
                auto cnt_storage_meta_key_buf = new NsExtContentStorageMetaKey[content_meta_count]();
                ScopeGuard on_exit([&]() {
                    delete[] cnt_storage_meta_key_buf;
                });
    
                u32 real_count;
                GLEAF_RC_TRY(nsextListApplicationRecordContentMeta(0, app.record.id, cnt_storage_meta_key_buf, content_meta_count, &real_count));
                content_storage_meta_keys.assign(cnt_storage_meta_key_buf, cnt_storage_meta_key_buf + real_count);
            }
    
            for(u32 i = 0; i < content_storage_meta_keys.size(); i++) {
                const auto &cnt_storage_meta_key = content_storage_meta_keys.at(i);
                if(cnt_storage_meta_key.meta_key.id == cnt.meta_key.id) {
                    content_storage_meta_keys.erase(content_storage_meta_keys.begin() + i);
                    break;
                }
            }
    
            GLEAF_RC_TRY(nsextDeleteApplicationRecord(app.record.id));
            if(!content_storage_meta_keys.empty()) {
                GLEAF_RC_TRY(nsextPushApplicationRecord(app.record.id, NsExtApplicationEvent_Present, content_storage_meta_keys.data(), content_storage_meta_keys.size()));
            }

            return 0;
        }

    }

    bool Application::GetIcon(u8 *&out_icon_data, size_t &out_icon_size) const {
        out_icon_data = nullptr;
        out_icon_size = 0;

        if(R_SUCCEEDED(GetApplicationControlData(this->record.id, g_TemporaryApplicationControlData, out_icon_size))) {
            out_icon_data = new u8[out_icon_size];
            memcpy(out_icon_data, reinterpret_cast<u8*>(g_TemporaryApplicationControlData.icon), out_icon_size);
            return true;
        }

        return false;
    }

    ApplicationPlayStats Application::GetGlobalPlayStats() const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationId(this->record.id, true, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }

    ApplicationPlayStats Application::GetUserPlayStats(const AccountUid user_id) const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(this->record.id, user_id, true, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }

    void InitializeApplications() {
        NotifyApplicationsChanged();
    }

    void FinalizeApplications() {
        g_LoadApplicationsThreadShouldExit = true;
        threadWaitForExit(&g_LoadApplicationsThread);
        threadClose(&g_LoadApplicationsThread);
    }

    void NotifyApplicationsChanged() {
        RequestLoadApplications();
    }

    std::vector<Application> &GetApplications() {
        ScopedLock lk(g_ApplicationsLock);
        return g_Applications;
    }

    std::optional<std::reference_wrapper<Application>> ExistsApplicationContent(const u64 program_id, const NcmContentMetaType content_type) {
        ScopedLock lk(g_ApplicationsLock);

        const auto app_id = GetBaseApplicationId(program_id, content_type);

        const auto app_it = std::find_if(g_Applications.begin(), g_Applications.end(), [&](const Application &app) -> bool {
            if(app.record.id == app_id) {
                const auto cnt_it = std::find_if(app.meta_status_list.begin(), app.meta_status_list.end(), [&](const NsApplicationContentMetaStatus &cnt_status) -> bool {
                    return (cnt_status.application_id == program_id) && (cnt_status.storageID != NcmStorageId_GameCard);
                });

                if(cnt_it != app.meta_status_list.end()) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        });
    
        if(app_it != g_Applications.end()) {
            return std::optional(std::reference_wrapper(*app_it));
        }
        else {
            return {};
        }
    }

    std::optional<std::reference_wrapper<Application>> ExistsApplicationAnyContents(const u64 program_id) {
        auto app = ExistsApplicationContent(program_id, NcmContentMetaType_Application);
        if(!app.has_value()) {
            app = ExistsApplicationContent(program_id, NcmContentMetaType_Patch);
        }
        if(!app.has_value()) {
            app = ExistsApplicationContent(program_id, NcmContentMetaType_AddOnContent);
        }

        return app;
    }

    void RemoveApplicationById(const u64 app_id) {
        /* TODO: old method of relying on NCM manually, is it even necessary?
        if(content.TryGetContents()) {
            NcmContentStorage cnt_storage = {};
            if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, content.storage_id))) {
                for(const auto &cnts: content.contents) {
                    for(u32 i = 0; i < ContentTypeCount; i++) {
                        if(!cnts.cnts[i].is_empty) {
                            ncmContentStorageDelete(&cnt_storage, &cnts.cnts[i].id);
                        }
                    }
                }
                ncmContentStorageClose(&cnt_storage);
            }
        }

        NcmContentMetaDatabase cnt_meta_db;
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, content.storage_id))) {
            ncmContentMetaDatabaseRemove(&cnt_meta_db, &content.meta_key);
            ncmContentMetaDatabaseCommit(&cnt_meta_db);
            
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }

        nsextDeleteApplicationRecord(content.program_id);
        nsDeleteApplicationCompletely(content.program_id);

        NotifyContentsChanged(content.storage_id);
        */

        auto rc = nsDeleteApplicationCompletely(app_id);
        if(R_FAILED(rc)) {
            GLEAF_LOG_FMT("Failed to delete application completely: 0x%X", rc);
        }
        rc = nsextDeleteApplicationRecord(app_id);
        if(R_FAILED(rc)) {
            GLEAF_LOG_FMT("Failed to delete application record: 0x%X", rc);
        }

        NotifyApplicationsChanged();
        g_MainApplication->GetApplicationListLayout()->NotifyApplicationsChanged();
    }

    void RemoveApplicationContentById(const cnt::Application &app, const u32 cnt_idx) {
        const auto &cnt_status = app.meta_status_list.at(cnt_idx);
        const auto &content = app.contents.at(cnt_idx);
        const auto cnt_storage_id = static_cast<NcmStorageId>(cnt_status.storageID);

        NcmContentStorage cnt_storage = {};
        if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, cnt_storage_id))) {
            for(u32 i = 0; i < MaxContentCount; i++) {
                if(content.cnt_ids[i].has_value()) {
                    ncmContentStorageDelete(&cnt_storage, std::addressof(content.cnt_ids[i].value()));
                }
            }
            ncmContentStorageClose(&cnt_storage);
        }

        NcmContentMetaDatabase cnt_meta_db;
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, cnt_storage_id))) {
            ncmContentMetaDatabaseRemove(&cnt_meta_db, &content.meta_key);
            ncmContentMetaDatabaseCommit(&cnt_meta_db);
            
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }

        const auto rc = RemoveApplicationContentMeta(app, content);
        if(R_FAILED(rc)) {
            GLEAF_LOG_FMT("Failed to remove application content meta: 0x%X", rc);
        }

        NotifyApplicationsChanged();
        g_MainApplication->GetApplicationListLayout()->NotifyApplicationsChanged();
    }

    Result UpdateApplicationVersion(const Application &app) {
        u32 existing_max_version = 0;
        for(const auto &status: app.meta_status_list) {
            if(status.version > existing_max_version) {
                existing_max_version = status.version;
            }
        }

        GLEAF_RC_TRY(avmInitialize());
        GLEAF_RC_TRY(avmPushLaunchVersion(app.record.id, existing_max_version));
        GLEAF_RC_TRY(avmUpgradeLaunchRequiredVersion(app.record.id, existing_max_version));
        avmExit();
        GLEAF_RC_TRY(nsextPushLaunchVersion(app.record.id, existing_max_version));
        GLEAF_RC_SUCCEED;
    }

    std::string GetExportedApplicationIconPath(const u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(GLEAF_PATH_TITLE_DIR "/" + util::FormatApplicationId(app_id) + ".jpg");
    }

    std::string GetExportedApplicationNacpPath(const u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(GLEAF_PATH_TITLE_DIR "/" + util::FormatApplicationId(app_id) + ".nacp");
    }

    std::string FindApplicationNacpName(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry = nullptr;
        nsGetApplicationDesiredLanguage(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->name;
        }
        else {
            return "<invalid>";
        }
    }

    std::string FindApplicationNacpAuthor(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry = nullptr;
        nsGetApplicationDesiredLanguage(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->author;
        }
        else {
            return "<invalid>";
        }
    }

}
