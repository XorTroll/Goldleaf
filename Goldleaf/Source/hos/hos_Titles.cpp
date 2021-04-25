
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

#include <hos/hos_Titles.hpp>
#include <hos/hos_Content.hpp>
#include <fs/fs_FileSystem.hpp>

namespace hos {

    namespace {

        TitlePlayStats ConvertPlayStats(PdmPlayStatistics play_stats) {
            TitlePlayStats stats = {};
            stats.total_play_secs = play_stats.playtimeMinutes * 60;
            const auto posix_time = time(nullptr);
            stats.secs_from_first_launched = posix_time - pdmPlayTimestampToPosix(play_stats.first_timestampUser);
            stats.secs_from_last_launched = posix_time - pdmPlayTimestampToPosix(play_stats.last_timestampUser);
            return stats;
        }

    }

    String ContentId::GetFileName() {
        return hos::ContentIdAsString(this->id) + ".nca";
    }

    String ContentId::GetFullPath() {
        NcmContentStorage cnt_storage;
        if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, static_cast<NcmStorageId>(this->location))))
        {
            char out_path[FS_MAX_PATH] = { 0 };
            if(R_SUCCEEDED(ncmContentStorageGetPath(&cnt_storage, out_path, FS_MAX_PATH, &this->id))) {
                return out_path;
            }
        }
        ncmContentStorageClose(&cnt_storage);
        return "";
    }

    u64 TitleContents::GetTotalSize() {
        return this->meta.size + this->program.size + this->data.size + this->control.size + this->html_document.size + this->legal_info.size;
    }

    NacpStruct Title::TryGetNACP() const {
        NsApplicationControlData control_data = {};
        size_t tmp = 0;
        if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, this->app_id, &control_data, sizeof(control_data), &tmp))) {
            return control_data.nacp;
        }
        else if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, GetBaseApplicationId(this->app_id, this->type), &control_data, sizeof(control_data), &tmp))) {
            return control_data.nacp;
        }
        return {};
    }

    u8 *Title::TryGetIcon() const {
        u8 *icon_data = nullptr;
        NsApplicationControlData control_data = {};
        size_t tmp = 0;
        if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, this->app_id, &control_data, sizeof(control_data), &tmp))) {
            icon_data = new u8[0x20000]();
            memcpy(icon_data, control_data.icon, 0x20000);
        }
        else if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, GetBaseApplicationId(this->app_id, this->type), &control_data, sizeof(control_data), &tmp))) {
            icon_data = new u8[0x20000]();
            memcpy(icon_data, control_data.icon, 0x20000);
        }
        return icon_data;
    }

    bool Title::DumpControlData() const {
        bool has_icon = false;
        const auto nacp = this->TryGetNACP();
        auto sdexp = fs::GetSdCardExplorer();
        if(!hos::IsNacpEmpty(nacp)) {
            const auto &nacp_file = GetExportedNACPPath(this->app_id);
            sdexp->DeleteFile(nacp_file);
            sdexp->WriteFile(nacp_file, &nacp, sizeof(nacp));
        }
        const auto icon_data = this->TryGetIcon();
        if(icon_data != nullptr) {
            const auto &icon_file = GetExportedIconPath(this->app_id);
            sdexp->DeleteFile(icon_file);
            sdexp->WriteFile(icon_file, icon_data, 0x20000);
            delete[] icon_data;
            has_icon = true;
        }
        return has_icon;
    }

    TitleContents Title::GetContents() const {
        TitleContents cnts = {};
        NcmContentMetaDatabase cnt_meta_db = {};
        NcmContentStorage cnt_storage = {};
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, static_cast<NcmStorageId>(this->location)))) {
            if(R_SUCCEEDED(ncmOpenContentStorage(&cnt_storage, static_cast<NcmStorageId>(this->location)))) {
                for(u32 i = 0; i < 6; i++) {
                    ContentId cnt_id = {};
                    cnt_id.type = static_cast<NcmContentType>(i);
                    cnt_id.is_empty = true;
                    cnt_id.size = 0;
                    cnt_id.location = this->location;
                    NcmContentId ipc_cnt_id;
                    if(R_SUCCEEDED(ncmContentMetaDatabaseGetContentIdByType(&cnt_meta_db, &ipc_cnt_id, &this->meta_key, cnt_id.type))) {
                        cnt_id.is_empty = false;
                        cnt_id.id = ipc_cnt_id;
                        s64 tmpsize = 0;
                        ncmContentStorageGetSizeFromContentId(&cnt_storage, &tmpsize, &ipc_cnt_id);
                        cnt_id.size = static_cast<u64>(tmpsize);
                    }

                    switch(i) {
                        case 0: {
                            cnts.meta = cnt_id;
                        }
                        case 1: {
                            cnts.program = cnt_id;
                        }
                        case 2: {
                            cnts.data = cnt_id;
                        }
                        case 3: {
                            cnts.control = cnt_id;
                        }
                        case 4: {
                            cnts.html_document = cnt_id;
                        }
                        case 5: {
                            cnts.legal_info = cnt_id;
                        }
                    }
                }
            }
        }
        ncmContentStorageClose(&cnt_storage);
        ncmContentMetaDatabaseClose(&cnt_meta_db);
        return cnts;
    }

    bool Title::IsBaseTitle() const {
        return (!this->IsUpdate()) && (!this->IsAddOnContent()) &&(this->type != NcmContentMetaType_SystemUpdate) && (this->type != NcmContentMetaType_Delta);
    }

    bool Title::IsUpdate() const {
        return this->type == NcmContentMetaType_Patch;
    }

    bool Title::IsAddOnContent() const {
        return this->type == NcmContentMetaType_AddOnContent;
    }

    bool Title::IsBaseOf(const Title &other) const {
        return (this->app_id != other.app_id) && ((GetBaseApplicationId(this->app_id, this->type) == GetBaseApplicationId(other.app_id, other.type)));
    }

    TitlePlayStats Title::GetGlobalPlayStats() const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationId(this->app_id, false, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }
    
    TitlePlayStats Title::GetUserPlayStats(AccountUid user_id) const {
        PdmPlayStatistics pdm_stats = {};
        pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(this->app_id, user_id, false, &pdm_stats);
        return ConvertPlayStats(pdm_stats);
    }

    u64 Ticket::GetApplicationId() const {
        return __builtin_bswap64(this->rights_id.app_id);
    }

    u64 Ticket::GetKeyGeneration() const {
        return __builtin_bswap64(this->rights_id.key_gen);
    }

    String Ticket::ToString() {
        const auto app_id = this->GetApplicationId();
        const auto key_gen = this->GetKeyGeneration();
        return FormatApplicationId(app_id) + FormatApplicationId(key_gen);
    }

    String TicketFile::GetTitleKey() const {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::hex;
        for(u32 i = 0; i < 0x10; i++) {
            strm << static_cast<u32>(this->data.title_key_block[i]);
        }
        return strm.str();
    }

    std::string FormatApplicationId(u64 app_id) {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << app_id;
        return strm.str();
    }

    std::vector<Title> SearchTitles(NcmContentMetaType type, Storage location) {
        std::vector<Title> titles;
        NcmContentMetaDatabase cnt_meta_db = {};
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, static_cast<NcmStorageId>(location)))) {
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
                        .location = location,
                    };
                    titles.push_back(title);
                }
            }
            delete[] meta_keys;
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }
        return titles;
    }

    Title Locate(u64 app_id) {
        #define _TMP_FIND_LOCATE { \
            const auto it = std::find_if(titles.begin(), titles.end(), [&](Title &t) -> bool { \
                return (t.app_id == app_id); \
            }); \
            if(it != titles.end()) { \
                title = *it; \
            } \
        }

        Title title = {};
        auto titles = SearchTitles(NcmContentMetaType_Unknown, Storage::NANDSystem);
        _TMP_FIND_LOCATE

        if(title.app_id == 0) {
            titles = SearchTitles(NcmContentMetaType_Unknown, Storage::NANDUser);
            _TMP_FIND_LOCATE
        }

        if(title.app_id == 0) {
            titles = SearchTitles(NcmContentMetaType_Unknown, Storage::SdCard);
            _TMP_FIND_LOCATE
        }

        #undef _TMP_FIND_LOCATE

        return title;
    }

    bool ExistsTitle(NcmContentMetaType type, Storage location, u64 app_id) {
        const auto ts = SearchTitles(type, location);

        const auto it = std::find_if(ts.begin(), ts.end(), [&](const Title &title) -> bool {
            return (title.app_id == app_id);
        });

        return (it != ts.end());
    }

    Result RemoveTitle(const Title &title) {
        const auto &cnts = title.GetContents();
        NcmContentStorage cnt_storage = {};
        auto rc = ncmOpenContentStorage(&cnt_storage, static_cast<NcmStorageId>(title.location));
        if(R_SUCCEEDED(rc)) {
            if(!cnts.meta.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.meta.id);
            }
            if(!cnts.program.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.program.id);
            }
            if(!cnts.data.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.data.id);
            }
            if(!cnts.control.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.control.id);
            }
            if(!cnts.html_document.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.html_document.id);
            }
            if(!cnts.legal_info.is_empty) {
                ncmContentStorageDelete(&cnt_storage, &cnts.legal_info.id);
            }
            ncmContentStorageClose(&cnt_storage);
        }
        NcmContentMetaDatabase cnt_meta_db;
        rc = ncmOpenContentMetaDatabase(&cnt_meta_db, static_cast<NcmStorageId>(title.location));
        if(R_SUCCEEDED(rc)) {
            rc = ncmContentMetaDatabaseRemove(&cnt_meta_db, &title.meta_key);
            if(R_SUCCEEDED(rc)) {
                ncmContentMetaDatabaseCommit(&cnt_meta_db);
            }
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }
        if(R_SUCCEEDED(rc)) {
            ns::DeleteApplicationRecord(title.app_id);
        }
        return rc;
    }

    Result RemoveTicket(const Ticket &tik) {
        return es::DeleteTicket(tik.rights_id);
    }

    std::vector<Ticket> GetAllTickets() {
        std::vector<Ticket> tickets;

        u32 common_count = 0;
        es::CountCommonTicket(&common_count);
        if(common_count > 0) {
            const auto ids_size = common_count * sizeof(es::RightsId);
            auto ids = new es::RightsId[common_count]();
            u32 written = 0;
            if(R_SUCCEEDED(es::ListCommonTicket(&written, ids, ids_size))) {
                for(u32 i = 0; i < written; i++) {
                    const Ticket common_tik = {
                        .rights_id = ids[i],
                        .type = hos::TicketType::Common,
                    };
                    tickets.push_back(common_tik);
                }
            }
            delete[] ids;
        }

        u32 personalized_count = 0;
        es::CountPersonalizedTicket(&personalized_count);
        if(personalized_count > 0) {
            const auto ids_size = personalized_count * sizeof(es::RightsId);
            auto ids = new es::RightsId[personalized_count]();
            u32 written = 0;
            if(R_SUCCEEDED(es::ListPersonalizedTicket(&written, ids, ids_size))) {
                for(u32 i = 0; i < written; i++) {
                    const Ticket personalized_tik = {
                        .rights_id = ids[i],
                        .type = hos::TicketType::Personalized,
                    };
                    tickets.push_back(personalized_tik);
                }
            }
            delete[] ids;
        }

        return tickets;
    }

    std::string GetExportedIconPath(u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(consts::Root + "/title/" + FormatApplicationId(app_id) + ".jpg").AsUTF8();
    }

    String GetExportedNACPPath(u64 app_id) {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(consts::Root + "/title/" + FormatApplicationId(app_id) + ".nacp").AsUTF8();
    }

    ApplicationIdMask GetApplicationIdMask(u64 app_id) {
        auto format_app_id = FormatApplicationId(app_id);
        auto id_start = format_app_id.substr(0, 2);
        if(id_start == "01") {
            return ApplicationIdMask::Official;
        }
        else if(id_start == "05") {
            return ApplicationIdMask::Homebrew;
        }
        return ApplicationIdMask::Invalid;
    }

    TicketFile ReadTicket(String path) {
        TicketFile tik = {};

        auto exp = fs::GetExplorerForPath(path);
        exp->StartFile(path, fs::FileMode::Read);
        exp->ReadFile(path, 0, sizeof(tik.signature), &tik.signature);

        const auto ticket_data_offset = GetTicketSignatureSize(tik.signature);
        exp->ReadFile(path, ticket_data_offset, sizeof(tik.data), &tik.data);

        exp->EndFile();
        return tik;
    }

    String FindNacpName(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry;
        nacpGetLanguageEntry(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->name;
        }
        return "";
    }

    String FindNacpAuthor(const NacpStruct &nacp) {
        NacpLanguageEntry *lang_entry;
        nacpGetLanguageEntry(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return lang_entry->author;
        }
        return "";
    }

}