
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

#pragma once
#include <base.hpp>
#include <fs/fs_FileSystem.hpp>

namespace cnt {

    NX_CONSTEXPR u64 GetBaseApplicationId(const u64 app_id, const NcmContentMetaType type) {
        switch(type) {
            case NcmContentMetaType_Patch: {
                return app_id ^ 0x800;
            }
            case NcmContentMetaType_AddOnContent: {
                return (app_id ^ 0x1000) & ~0xFFF;
            }
            default: {
                return app_id;
            }
        }
    }

    enum class Source {
        SdCard,
        BuiltInUser,
        GameCard,

        Count
    };

    NX_CONSTEXPR NcmStorageId GetStorageIdFromSource(const Source src) {
        switch(src) {
            case Source::SdCard: {
                return NcmStorageId_SdCard;
            }
            case Source::BuiltInUser: {
                return NcmStorageId_BuiltInUser;
            }
            case Source::GameCard: {
                return NcmStorageId_GameCard;
            }
            default: {
                GLEAF_ASSERT_FAIL("Invalid source");
            }
        }
    }

    struct ApplicationPlayStats {
        u64 total_play_secs;
        u64 secs_from_last_launched;
        u64 secs_from_first_launched;
    };

    constexpr size_t MaxContentCount = 7;

    struct ApplicationContentEntry {
        std::optional<NcmContentId> cnt_ids[MaxContentCount];
    };

    struct ApplicationCache {
        std::string display_name;
        std::string display_author;
        std::string icon_path;
        std::string record_last_event;
        std::vector<std::string> view_flags;
    };

    struct Application {
        NsExtApplicationRecord record;
        NsExtApplicationView view;
        NsApplicationControlData control_data;
        std::vector<NsApplicationContentMetaStatus> meta_status_list;
        std::vector<ApplicationContentEntry> contents;
        NsExtApplicationOccupiedSize occupied_size;
        u32 max_version;
        u32 launch_required_version;
        ApplicationCache cache;

        inline Application() {}

        // Prevent unwanted copying, this is a large struct
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = default;
        Application& operator=(Application&&) = default;

        bool DumpControlData();

        ApplicationPlayStats GetGlobalPlayStats() const;
        ApplicationPlayStats GetUserPlayStats(const AccountUid user_id) const;
    };

    constexpr u32 MaxApplicationCount = 64000;

    void InitializeApplications();
    void FinalizeApplications();

    void NotifyApplicationsChanged();

    std::vector<Application> &GetApplications();
    std::optional<std::reference_wrapper<Application>> ExistsApplicationContent(const u64 app_id);
    
    void RemoveApplicationById(const u64 app_id);
    Result UpdateApplicationVersion(const Application &app);
    std::string GetExportedApplicationIconPath(const u64 app_id);
    std::string GetExportedApplicationNacpPath(const u64 app_id);

    NX_CONSTEXPR u32 GetAddOnContentId(const u64 app_id) {
        return app_id & 0xFFF;
    }

    NX_CONSTEXPR bool IsApplicationNacpEmpty(const NacpStruct &nacp) {
        // Check a field which is basically guaranteed to be non-empty for a valid NACP
        return strlen(nacp.display_version) == 0;
    }

    std::string FindApplicationNacpName(const NacpStruct &nacp);
    std::string FindApplicationNacpAuthor(const NacpStruct &nacp);

}
