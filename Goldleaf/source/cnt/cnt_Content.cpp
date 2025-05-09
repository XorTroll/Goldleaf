
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
                            NcmContentMetaKey meta_key;
                            if(R_SUCCEEDED(ncmContentMetaDatabaseGetLatestContentMetaKey(&meta_db, &meta_key, status.application_id))) {
                                for(u32 i = 0; i < MaxContentCount; i++) {
                                    NcmContentId cnt_id;
                                    if(R_SUCCEEDED(ncmContentMetaDatabaseGetContentIdByType(&meta_db, &cnt_id, &meta_key, static_cast<NcmContentType>(i)))) {
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

        /*
        bool SortContentsImpl(cnt::ContentEntry &cnt_a, cnt::ContentEntry &cnt_b) {
            const auto cnt_a_name = cnt_a.TryGetNacp() ? cnt::FindNacpName(cnt_a.nacp) : "";   
            const auto cnt_b_name = cnt_b.TryGetNacp() ? cnt::FindNacpName(cnt_b.nacp) : "";

            if(cnt_a_name.empty() && !cnt_b_name.empty()) {
                return true;
            }
            if(!cnt_a_name.empty() && cnt_b_name.empty()) {
                return false;
            }
            if(cnt_a_name.empty() && cnt_b_name.empty()) {
                return cnt_a.program_id < cnt_b.program_id;
            }

            return cnt_a_name.front() < cnt_b_name.front();
        }
        */

        void ScanApplications() {
            ScopedLock lk(g_ApplicationsLock);
            g_Applications.clear();

            // Scan application records and meta status
            ScanApplicationRecords();

            // Try to get application views and control data
            for(auto &app: g_Applications) {
                if(R_SUCCEEDED(nsGetApplicationView(reinterpret_cast<NsApplicationView*>(&app.view), &app.record.id, 1))) {
                    for(u32 i = 0; i < sizeof(app.view.flags) * CHAR_BIT; i++) {
                        if(app.view.flags & BIT(i)) {
                            app.cache.view_flags.push_back(GetApplicationViewFlagName(i));
                        }
                    }
                }

                u64 dummy_size;
                if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, app.record.id, &app.control_data, sizeof(app.control_data), &dummy_size))) {
                    app.cache.display_name = FindApplicationNacpName(app.control_data.nacp);
                    app.cache.display_author = FindApplicationNacpAuthor(app.control_data.nacp);
                }
                else {
                    app.cache.display_name = util::FormatApplicationId(app.record.id);
                    app.cache.display_author = "";
                }

                nsCalculateApplicationOccupiedSize(app.record.id, reinterpret_cast<NsApplicationOccupiedSize*>(&app.occupied_size));

                GLEAF_LOG_FMT("Application %016lX:", app.record.id);

                if(!IsApplicationNacpEmpty(app.control_data.nacp)) {
                    GLEAF_LOG_FMT("  - NACP title: %s", FindApplicationNacpName(app.control_data.nacp).c_str());
                    GLEAF_LOG_FMT("  - NACP author: %s", FindApplicationNacpAuthor(app.control_data.nacp).c_str());
                    GLEAF_LOG_FMT("  - NACP version: %s", app.control_data.nacp.display_version);
                }
                else {
                    GLEAF_LOG_FMT("  ! <no NACP>");
                }

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

            // std::sort(contents.begin(), contents.end(), SortContentsImpl);
        }

        Thread g_LoadApplicationsThread;

        void LoadApplicationsMain(void*) {
            SetThreadName("cnt.LoadApplicationsThread");

            GLEAF_LOG_FMT("Scanning applications...");
            ScanApplications();
            GLEAF_LOG_FMT("Done! Exiting thread...");
        }

        void RequestLoadApplications() {
            threadClose(&g_LoadApplicationsThread);
            GLEAF_RC_ASSERT(threadCreate(&g_LoadApplicationsThread, LoadApplicationsMain, nullptr, nullptr, 512_KB, 0x1F, -2));
            GLEAF_RC_ASSERT(threadStart(&g_LoadApplicationsThread));
        }

    }

    bool Application::DumpControlData() {
        if(!IsApplicationNacpEmpty(this->control_data.nacp)) {
            auto sd_exp = fs::GetSdCardExplorer();
            const auto &nacp_file = GetExportedApplicationNacpPath(this->record.id);
            sd_exp->DeleteFile(nacp_file);
            sd_exp->WriteFile(nacp_file, &this->control_data.nacp, sizeof(this->control_data.nacp));

            this->cache.icon_path = GetExportedApplicationIconPath(this->record.id);
            sd_exp->DeleteFile(this->cache.icon_path);
            sd_exp->WriteFile(this->cache.icon_path, this->control_data.icon, sizeof(this->control_data.icon));
            return true;
        }
        else {
            return false;
        }
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
        threadClose(&g_LoadApplicationsThread);
    }

    void NotifyApplicationsChanged() {
        RequestLoadApplications();
    }

    std::vector<Application> &GetApplications() {
        ScopedLock lk(g_ApplicationsLock);
        return g_Applications;
    }

    std::optional<std::reference_wrapper<Application>> ExistsApplicationContent(const u64 app_id) {
        ScopedLock lk(g_ApplicationsLock);
        const auto app_it = std::find_if(g_Applications.begin(), g_Applications.end(), [&](const Application &app) -> bool {
            return (app.record.id == app_id) || (app.record.id == GetBaseApplicationId(app_id, NcmContentMetaType_Patch)) || (app.record.id == GetBaseApplicationId(app_id, NcmContentMetaType_AddOnContent));
        });
        
        if(app_it != g_Applications.end()) {
            for(const auto &cnt_status: app_it->meta_status_list) {
                if(cnt_status.application_id == app_id) {
                    return std::optional(std::reference_wrapper(*app_it));
                }
            }
        }

        return {};
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

        nsextDeleteApplicationRecord(app_id);
        nsDeleteApplicationCompletely(app_id);
        NotifyApplicationsChanged();
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
