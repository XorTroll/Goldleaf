#include <cnt/cnt_Names.hpp>
#include <cfg/cfg_Strings.hpp>

namespace cnt {

    std::string GetContentMetaTypeName(const NcmContentMetaType type) {
        switch(type) {
            case NcmContentMetaType_Unknown: {
                return "Unknown";
            }
            case NcmContentMetaType_SystemProgram: {
                return "SystemProgram";
            }
            case NcmContentMetaType_SystemData: {
                return "SystemData";
            }
            case NcmContentMetaType_SystemUpdate: {
                return "SystemUpdate";
            }
            case NcmContentMetaType_BootImagePackage: {
                return "BootImagePackage";
            }
            case NcmContentMetaType_BootImagePackageSafe: {
                return "BootImagePackageSafe";
            }
            case NcmContentMetaType_Application: {
                return cfg::Strings.GetString(261);
            }
            case NcmContentMetaType_Patch: {
                return cfg::Strings.GetString(262);
            }
            case NcmContentMetaType_AddOnContent: {
                return cfg::Strings.GetString(263);
            }
            case NcmContentMetaType_Delta: {
                return "Delta";
            }
            case NcmContentMetaType_DataPatch: {
                return "DataPatch";
            }
            default: {
                return "Unk" + std::to_string(type);
            }
        }
    }

    std::string GetContentTypeName(const NcmContentType type) {
        switch(type) {
            case NcmContentType_Meta: {
                return cfg::Strings.GetString(163);
            }
            case NcmContentType_Program: {
                return cfg::Strings.GetString(164);
            }
            case NcmContentType_Data: {
                return cfg::Strings.GetString(165);
            }
            case NcmContentType_Control: {
                return cfg::Strings.GetString(166);
            }
            case NcmContentType_HtmlDocument: {
                return cfg::Strings.GetString(167);
            }
            case NcmContentType_LegalInformation: {
                return cfg::Strings.GetString(168);
            }
            case NcmContentType_DeltaFragment: {
                return cfg::Strings.GetString(513);
            }
            default: {
                return "Unk" + std::to_string(type);
            }
        }
    }

    std::string GetStorageIdName(const NcmStorageId id) {
        switch(id) {
            case NcmStorageId_SdCard: {
                return cfg::Strings.GetString(19);
            }
            case NcmStorageId_BuiltInUser: {
                return cfg::Strings.GetString(28);
            }
            case NcmStorageId_BuiltInSystem: {
                return cfg::Strings.GetString(29);
            }
            case NcmStorageId_GameCard: {
                return cfg::Strings.GetString(190);
            }
            case NcmStorageId_None: {
                return cfg::Strings.GetString(442);
            }
            default: {
                return "Unk" + std::to_string(id);
            }
        }
    }

    std::string GetApplicationViewFlagName(const u32 flag_bit) {
        switch(BIT(flag_bit)) {
            case NsExtApplicationViewFlag_IsValid: {
                return "IsValid";
            }
            case NsExtApplicationViewFlag_HasMainContents: {
                return "HasMainContents";
            }
            case NsExtApplicationViewFlag_HasContentsInstalled: {
                return "HasContentsInstalled";
            }
            case NsExtApplicationViewFlag_IsDownloading: {
                return "IsDownloading";
            }
            case NsExtApplicationViewFlag_IsGameCard: {
                return "IsGameCard";
            }
            case NsExtApplicationViewFlag_IsGameCardInserted: {
                return "IsGameCardInserted";
            }
            case NsExtApplicationViewFlag_CanLaunch: {
                return "CanLaunch";
            }
            case NsExtApplicationViewFlag_NeedsUpdate: {
                return "NeedsUpdate";
            }
            case NsExtApplicationViewFlag_CanLaunch2: {
                return "CanLaunch2";
            }
            case NsExtApplicationViewFlag_NeedsVerify: {
                return "NeedsVerify";
            }
            case NsExtApplicationViewFlag_IsWaitingCommit1: {
                return "IsWaitingCommit1";
            }
            case NsExtApplicationViewFlag_IsWaitingCommit2: {
                return "IsWaitingCommit2";
            }
            case NsExtApplicationViewFlag_IsApplyingDelta: {
                return "IsApplyingDelta";
            }
            case NsExtApplicationViewFlag_IsWaitingCommit3: {
                return "IsWaitingCommit3";
            }
            default: {
                return "Unk" + std::to_string(flag_bit);
            }
        }
    }

    std::string GetApplicationEventName(const NsExtApplicationEvent event) {
        switch(event) {
            case NsExtApplicationEvent_Launched: {
                return "Launched";
            }
            case NsExtApplicationEvent_LocalInstalled: {
                return "LocalInstalled";
            }
            case NsExtApplicationEvent_DownloadStarted: {
                return "DownloadStarted";
            }
            case NsExtApplicationEvent_Present: {
                return "Present";
            }
            case NsExtApplicationEvent_Touched: {
                return "Touched";
            }
            case NsExtApplicationEvent_GamecardMissing: {
                return "GamecardMissing";
            }
            case NsExtApplicationEvent_Downloaded: {
                return "Downloaded";
            }
            case NsExtApplicationEvent_Updated: {
                return "Updated";
            }
            case NsExtApplicationEvent_Archived: {
                return "Archived";
            }
            case NsExtApplicationEvent_UpdateNeeded: {
                return "UpdateNeeded";
            }
            case NsExtApplicationEvent_AlreadyStarted: {
                return "AlreadyStarted";
            }
            default: {
                return "Unk" + std::to_string(event);
            }
        }
    }

}
