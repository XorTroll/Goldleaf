
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

#include <hos/hos_Titles.hpp>
#include <hos/hos_Content.hpp>
#include <fs/fs_FileSystem.hpp>

namespace hos {

    namespace {

        inline TitlePlayStats ConvertPlayStats(const PdmPlayStatistics play_stats) {
            const auto posix_time = time(nullptr);

            return {
                .total_play_secs = (u64)(((double)play_stats.playtime) / 1.0e9),
                .secs_from_last_launched = posix_time - play_stats.last_timestamp_user,
                .secs_from_first_launched = posix_time - play_stats.first_timestamp_user
            };
        }

        // Note: placed here since it might be too big to just be loaded on the stack
        NsApplicationControlData g_TempApplicationControlData;

        bool LoadApplicationControlData(const u64 app_id) {
            size_t tmp;
            return R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, app_id, &g_TempApplicationControlData, sizeof(g_TempApplicationControlData), &tmp));
        }

        std::unordered_map<NcmStorageId, std::vector<Title>> g_Titles;
        std::unordered_map<NcmStorageId, bool> g_TitlesNeedRefresh;

    }

    std::string ContentId::GetBaseName() {
        return ContentIdAsString(this->id) + ".nca";
    }

    std::string ContentId::GetFullPath() {
        NcmContentStorage cnt_storage;
        if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, this->storage_id))) {
            ScopeGuard on_exit([&]() {
                ncmContentStorageClose(&cnt_storage);
            });

            char out_path[FS_MAX_PATH] = {};
            if(R_SUCCEEDED(ncmContentStorageGetPath(&cnt_storage, out_path, FS_MAX_PATH, &this->id))) {
                return out_path;
            }
        }
        return "";
    }

    bool Title::TryGetNacp() {
        if(!IsNacpEmpty(this->nacp)) {
            return true;
        }

        if(LoadApplicationControlData(this->app_id) || LoadApplicationControlData(GetBaseApplicationId(this->app_id, this->type))) {
            memcpy(&this->nacp, &g_TempApplicationControlData.nacp, sizeof(this->nacp));
            return true;
        }
        else {
            return false;
        }
    }

    bool Title::TryGetIcon(u8 *&out_icon) const {
        if(LoadApplicationControlData(this->app_id) || LoadApplicationControlData(GetBaseApplicationId(this->app_id, this->type))) {
            out_icon = new u8[IconDataSize]();
            memcpy(out_icon, g_TempApplicationControlData.icon, IconDataSize);
            return true;
        }
        else {
            return false;
        }
    }

    bool Title::DumpControlData() {
        auto sd_exp = fs::GetSdCardExplorer();

        if(this->TryGetNacp()) {
            const auto &nacp_file = GetExportedNacpPath(this->app_id);
            sd_exp->DeleteFile(nacp_file);
            sd_exp->WriteFile(nacp_file, &this->nacp, sizeof(this->nacp));
        }

        bool has_icon = false;
        u8 *icon_data;
        if(this->TryGetIcon(icon_data)) {
            const auto &icon_file = GetExportedIconPath(this->app_id);
            sd_exp->DeleteFile(icon_file);
            sd_exp->WriteFile(icon_file, icon_data, IconDataSize);
            delete[] icon_data;
            has_icon = true;
        }
        return has_icon;
    }

    bool Title::TryGetContents() {
        if(this->title_cnts.empty()) {
            u8 idx = 0;
            while(true) {
                auto found_any = false;
                TitleContents cnts = {};
                NcmContentMetaDatabase cnt_meta_db = {};
                NcmContentStorage cnt_storage = {};
                if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, this->storage_id))) {
                    if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, this->storage_id))) {
                        for(u32 i = 0; i < ncm::ContentTypeCount; i++) {
                            cnts.cnts[i] = {
                                .type = static_cast<NcmContentType>(i),
                                .storage_id = this->storage_id,
                                .is_empty = true,
                                .size = 0
                            };
            
                            if(R_SUCCEEDED(ncmContentMetaDatabaseGetContentIdByTypeAndIdOffset(&cnt_meta_db, &cnts.cnts[i].id, &this->meta_key, cnts.cnts[i].type, idx))) {
                                cnts.cnts[i].is_empty = false;
                                s64 tmp_size = 0;
                                ncmContentStorageGetSizeFromContentId(&cnt_storage, &tmp_size, &cnts.cnts[i].id);
                                cnts.cnts[i].size = static_cast<u64>(tmp_size);
                                found_any = true;
                            }
                        }
                        ncmContentStorageClose(&cnt_storage);
                    }
                    ncmContentMetaDatabaseClose(&cnt_meta_db);
                }
                if(!found_any) {
                    break;
                }

                idx++;
                this->title_cnts.push_back(cnts);
            }
            
        }
    
        return !this->title_cnts.empty();
    }

    TitlePlayStats Title::GetGlobalPlayStats() const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationId(this->app_id, true, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }
    
    TitlePlayStats Title::GetUserPlayStats(const AccountUid user_id) const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(this->app_id, user_id, true, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }

    bool Ticket::IsUsed() const {
        return ExistsTitle(NcmStorageId_SdCard, NcmContentMetaType_Unknown, this->rights_id.GetApplicationId()) || ExistsTitle(NcmStorageId_BuiltInUser, NcmContentMetaType_Unknown, this->rights_id.GetApplicationId());
    }

    std::string Ticket::ToString() const {
        return FormatApplicationId(this->rights_id.GetApplicationId()) + FormatApplicationId(this->rights_id.GetKeyGeneration());
    }

    std::string TicketData::GetTitleKey() const {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::hex;
        for(u32 i = 0; i < 0x10; i++) {
            strm << static_cast<u32>(this->title_key_block[i]);
        }
        return strm.str();
    }

    void Initialize() {
        g_TitlesNeedRefresh[NcmStorageId_SdCard] = true;
        g_TitlesNeedRefresh[NcmStorageId_BuiltInUser] = true;
        g_TitlesNeedRefresh[NcmStorageId_BuiltInSystem] = true;
        g_TitlesNeedRefresh[NcmStorageId_GameCard] = true;
        g_TitlesNeedRefresh[NcmStorageId_None] = true;
    }

    void NotifyTitlesChanged(const NcmStorageId storage_id) {
        g_TitlesNeedRefresh[storage_id] = true;
    }

    std::string FormatApplicationId(const u64 app_id) {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << app_id;
        return strm.str();
    }

    std::vector<Title> &SearchTitles(const NcmStorageId storage_id, const NcmContentMetaType type) {
        if(g_TitlesNeedRefresh.at(storage_id)) {
            std::vector<Title> titles;
            // Treat 'no storage' as a way of find titles without any content (only records, thus not present at meta databases)
            if(storage_id == NcmStorageId_None) {
                auto app_records = new NsApplicationRecord[MaxTitleCount]();
                s32 record_count;
                if(R_SUCCEEDED(nsListApplicationRecord(app_records, MaxTitleCount, 0, &record_count))) {
                    for(s32 i = 0; i < record_count; i++) {
                        const auto app_record = app_records[i];
                        NsApplicationContentMetaStatus tmp_meta_status;
                        s32 tmp_count;
                        if(R_SUCCEEDED(nsListApplicationContentMetaStatus(app_record.application_id, 0, &tmp_meta_status, 1, &tmp_count)) && (tmp_count > 0)) {
                            if(tmp_meta_status.storageID == storage_id) {
                                const Title title = {
                                    .app_id = tmp_meta_status.application_id,
                                    .type = static_cast<NcmContentMetaType>(tmp_meta_status.meta_type),
                                    .version = tmp_meta_status.version,
                                    .storage_id = storage_id
                                };
                                titles.push_back(title);
                            }
                        }
                    }
                }
                delete[] app_records;
            }
            else {
                NcmContentMetaDatabase cnt_meta_db = {};
                if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, storage_id))) {
                    auto meta_keys = new NcmContentMetaKey[MaxTitleCount]();
                    s32 written = 0;
                    s32 total = 0;
                    if(R_SUCCEEDED(ncmContentMetaDatabaseList(&cnt_meta_db, &total, &written, meta_keys, MaxTitleCount, type, 0, 0, UINT64_MAX, NcmContentInstallType_Full)) && (written > 0)) {
                        for(s32 i = 0; i < written; i++) {
                            const auto cur_meta_key = meta_keys[i];
                            const Title title = {
                                .app_id = cur_meta_key.id,
                                .type = static_cast<NcmContentMetaType>(cur_meta_key.type),
                                .version = cur_meta_key.version,
                                .meta_key = cur_meta_key,
                                .storage_id = storage_id,
                            };
                            titles.push_back(title);
                        }
                    }
                    delete[] meta_keys;
                    ncmContentMetaDatabaseClose(&cnt_meta_db);
                }
            }

            g_Titles[storage_id] = titles;
            g_TitlesNeedRefresh[storage_id] = false;
        }

        return g_Titles.at(storage_id);
    }

    Title Locate(const u64 app_id) {
        #define _TMP_FIND_LOCATE { \
            const auto it = std::find_if(titles.begin(), titles.end(), [&](Title &t) -> bool { \
                return (t.app_id == app_id); \
            }); \
            if(it != titles.end()) { \
                title = *it; \
            } \
        }

        Title title = {};
        auto &titles = SearchTitles(NcmStorageId_BuiltInSystem, NcmContentMetaType_Unknown);
        _TMP_FIND_LOCATE

        if(title.app_id == 0) {
            auto &titles = SearchTitles(NcmStorageId_BuiltInUser, NcmContentMetaType_Unknown);
            _TMP_FIND_LOCATE
        }

        if(title.app_id == 0) {
            auto &titles = SearchTitles(NcmStorageId_SdCard, NcmContentMetaType_Unknown);
            _TMP_FIND_LOCATE
        }

        #undef _TMP_FIND_LOCATE

        return title;
    }

    bool ExistsTitle(const NcmStorageId storage_id, const NcmContentMetaType type, const u64 app_id) {
        const auto &titles = SearchTitles(storage_id, type);

        const auto title_it = std::find_if(titles.begin(), titles.end(), [&](const Title &title) -> bool {
            return (title.app_id == app_id);
        });
        return title_it != titles.end();
    }

    void RemoveTitle(Title &title) {
        if(title.TryGetContents()) {
            for(const auto &cnts: title.title_cnts) {
                NcmContentStorage cnt_storage = {};
                if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, title.storage_id))) {
                    for(u32 i = 0; i < ncm::ContentTypeCount; i++) {
                        if(!cnts.cnts[i].is_empty) {
                            ncmContentStorageDelete(&cnt_storage, &cnts.cnts[i].id);
                        }
                    }
                    ncmContentStorageClose(&cnt_storage);
                }
            }
        }

        NcmContentMetaDatabase cnt_meta_db;
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, title.storage_id))) {
            ncmContentMetaDatabaseRemove(&cnt_meta_db, &title.meta_key);
            ncmContentMetaDatabaseCommit(&cnt_meta_db);
            
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }

        ns::DeleteApplicationRecord(title.app_id);
        nsDeleteApplicationCompletely(title.app_id);

        NotifyTitlesChanged(title.storage_id);
    }

    Result RemoveTicket(const Ticket &tik) {
        return es::DeleteTicket(tik.rights_id);
    }

    Result UpdateTitleVersion(const Title &title) {
        const auto &titles = SearchTitles(title.storage_id, NcmContentMetaType_Unknown);
        auto cur_max_version = title.meta_key.version;
        for(const auto &search_title: titles) {
            if(title.IsBaseOf(search_title)) {
                if(search_title.meta_key.version > cur_max_version) {
                    cur_max_version = search_title.meta_key.version;
                }
            }
        }

        GLEAF_RC_TRY(avmPushLaunchVersion(title.app_id, cur_max_version));
        GLEAF_RC_TRY(avmUpgradeLaunchRequiredVersion(title.app_id, cur_max_version));
        GLEAF_RC_SUCCEED;
    }

    std::vector<Ticket> GetAllTickets() {
        std::vector<Ticket> tickets;

        const auto common_count = es::CountCommonTicket();
        if(common_count > 0) {
            const auto ids_size = common_count * sizeof(es::RightsId);
            auto ids = new es::RightsId[common_count]();
            u32 written = 0;
            if(R_SUCCEEDED(es::ListCommonTicket(&written, ids, ids_size))) {
                for(u32 i = 0; i < written; i++) {
                    const Ticket common_tik = {
                        .rights_id = ids[i],
                        .type = TicketType::Common
                    };
                    tickets.push_back(common_tik);
                }
            }
            delete[] ids;
        }

        const auto personalized_count = es::CountPersonalizedTicket();
        if(personalized_count > 0) {
            const auto ids_size = personalized_count * sizeof(es::RightsId);
            auto ids = new es::RightsId[personalized_count]();
            u32 written = 0;
            if(R_SUCCEEDED(es::ListPersonalizedTicket(&written, ids, ids_size))) {
                for(u32 i = 0; i < written; i++) {
                    const Ticket personalized_tik = {
                        .rights_id = ids[i],
                        .type = TicketType::Personalized
                    };
                    tickets.push_back(personalized_tik);
                }
            }
            delete[] ids;
        }

        return tickets;
    }

    std::string GetExportedIconPath(const u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(GLEAF_PATH_TITLE_DIR "/" + FormatApplicationId(app_id) + ".jpg");
    }

    std::string GetExportedNacpPath(const u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(GLEAF_PATH_TITLE_DIR "/" + FormatApplicationId(app_id) + ".nacp");
    }

    ApplicationIdMask GetApplicationIdMask(const u64 app_id) {
        const auto format_app_id = FormatApplicationId(app_id);
        const auto id_start = format_app_id.substr(0, 2);
        if(id_start == "01") {
            return ApplicationIdMask::Official;
        }
        else if(id_start == "05") {
            return ApplicationIdMask::Homebrew;
        }
        else {
            return ApplicationIdMask::Invalid;
        }
    }

    TicketFile ReadTicket(const std::string &path) {
        TicketFile tik_file = {};

        auto exp = fs::GetExplorerForPath(path);
        exp->StartFile(path, fs::FileMode::Read);
        exp->ReadFile(path, 0, sizeof(tik_file.signature), &tik_file.signature);

        const auto tik_sig_data_size = GetTicketSignatureDataSize(tik_file.signature);
        exp->ReadFile(path, sizeof(tik_file.signature), tik_sig_data_size, tik_file.signature_data);

        const auto tik_data_offset = GetTicketSignatureSize(tik_file.signature);
        exp->ReadFile(path, tik_data_offset, sizeof(tik_file.data), &tik_file.data);

        exp->EndFile();
        return tik_file;
    }

    void SaveTicket(fs::Explorer *exp, const std::string &path, const TicketFile tik_file) {
        exp->DeleteFile(path);

        exp->StartFile(path, fs::FileMode::Append);
        exp->WriteFile(path, &tik_file.signature, sizeof(tik_file.signature));
        exp->WriteFile(path, tik_file.signature_data, GetTicketSignatureDataSize(tik_file.signature));
        exp->WriteFile(path, &tik_file.data, sizeof(tik_file.data));
        exp->EndFile();
    }

    std::string FindNacpName(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry;
        nsGetApplicationDesiredLanguage(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->name;
        }
        else {
            return "(...)";
        }
    }

    std::string FindNacpAuthor(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry;
        nsGetApplicationDesiredLanguage(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->author;
        }
        else {
            return "(...)";
        }
    }

}