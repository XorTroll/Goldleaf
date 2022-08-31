
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

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <base_Common.hpp>
#include <ncm/ncm_Types.hpp> 
#include <ns/ns_Service.hpp>
#include <es/es_Service.hpp>
#include <fs/fs_FileSystem.hpp>

namespace hos {

    constexpr size_t IconDataSize = 0x20000;

    enum class TicketType {
        Common,
        Personalized,
    };

    enum class TicketSignature : u32 {
        Invalid,
        RSA_4096_SHA1 = 0x10000,
        RSA_2048_SHA1 = 0x10001,
        ECDSA_SHA1 = 0x10002,
        RSA_4096_SHA256 = 0x10003,
        RSA_2048_SHA256 = 0x10004,
        ECDSA_SHA256 = 0x10005,
    };

    inline constexpr bool IsValidTicketSignature(const TicketSignature sig) {
        return (sig >= TicketSignature::RSA_4096_SHA1) && (sig <= TicketSignature::ECDSA_SHA256);
    }

    inline constexpr u64 GetBaseApplicationId(const u64 app_id, const NcmContentMetaType type) {
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

    enum class ApplicationIdMask {
        Official,
        Homebrew,
        Invalid,
    };

    struct ContentId {
        NcmContentType type;
        NcmContentId id;
        NcmStorageId storage_id;
        bool is_empty;
        u64 size;

        std::string GetFileName();
        std::string GetFullPath();
    };

    struct TitleContents {
        ContentId meta;
        ContentId program;
        ContentId data;
        ContentId control;
        ContentId html_document;
        ContentId legal_info;

        inline u64 GetTotalSize() const {
            return this->meta.size + this->program.size + this->data.size + this->control.size + this->html_document.size + this->legal_info.size;
        }
    };

    struct TitlePlayStats {
        u64 total_play_secs;
        u64 secs_from_last_launched;
        u64 secs_from_first_launched;
    };

    struct Title {
        u64 app_id;
        NcmContentMetaType type;
        u32 version;
        NcmContentMetaKey meta_key;
        NcmStorageId storage_id;
        
        NacpStruct TryGetNACP() const;
        u8 *TryGetIcon() const;
        bool DumpControlData() const;
        TitleContents GetContents() const;

        inline bool IsBaseTitle() const {
            return (!this->IsUpdate()) && (!this->IsAddOnContent()) && (this->type != NcmContentMetaType_SystemUpdate) && (this->type != NcmContentMetaType_Delta);
        }

        inline bool IsUpdate() const {
            return this->type == NcmContentMetaType_Patch;
        }

        inline bool IsAddOnContent() const {
            return this->type == NcmContentMetaType_AddOnContent;
        }

        inline bool IsBaseOf(const Title &other) const {
            return (this->app_id != other.app_id) && ((GetBaseApplicationId(this->app_id, this->type) == GetBaseApplicationId(other.app_id, other.type)));
        }

        TitlePlayStats GetGlobalPlayStats() const;
        TitlePlayStats GetUserPlayStats(const AccountUid user_id) const;
    };

    struct Ticket {
        es::RightsId rights_id;
        TicketType type;

        bool IsUsed() const;
        std::string ToString() const;
    };

    struct TicketData {
        u8 issuer[0x40];
        u8 title_key_block[0x100];
        u8 unk[0x6];
        u8 master_key_gen;
        u8 unk_2;
        u8 unk_3[0x8];
        u8 ticket_id[0x8];
        u8 device_id[0x8];
        es::RightsId rights_id;
        u8 account_id[0x4];
        u8 unk_4[0xC];

        std::string GetTitleKey() const;
    };
    static_assert(sizeof(TicketData) == 0x180);

    inline constexpr u64 GetTicketSignatureDataSize(const TicketSignature sig) {
        switch(sig) {
            case TicketSignature::RSA_4096_SHA1:
            case TicketSignature::RSA_4096_SHA256: {
                return 0x200 + 0x3C;
            }
            case TicketSignature::RSA_2048_SHA1:
            case TicketSignature::RSA_2048_SHA256: {
                return 0x100 + 0x3C;
            }
            case TicketSignature::ECDSA_SHA1:
            case TicketSignature::ECDSA_SHA256: {
                return 0x3C + 0x40;
            }
            default: {
                return 0;
            }
        }
    }
    
    inline constexpr u64 GetTicketSignatureSize(const TicketSignature sig) {
        return sizeof(sig) + GetTicketSignatureDataSize(sig);
    }

    struct TicketFile {
        TicketSignature signature;
        u8 signature_data[0x300];
        TicketData data;

        inline constexpr u64 GetFullSize() {
            return GetTicketSignatureSize(this->signature) + sizeof(this->data);
        }
    };

    constexpr u32 MaxTitleCount = 64000;

    std::string FormatApplicationId(const u64 app_id);
    std::vector<Title> SearchTitles(const NcmContentMetaType type, const NcmStorageId storage_id);
    Title Locate(const u64 app_id);
    bool ExistsTitle(const NcmContentMetaType type, const NcmStorageId storage_id, const u64 app_id);
    std::vector<Ticket> GetAllTickets();
    Result RemoveTitle(const Title &title);
    Result RemoveTicket(const Ticket &tik);
    Result UpdateTitleVersion(const Title &title);
    std::string GetExportedIconPath(const u64 app_id);
    std::string GetExportedNacpPath(const u64 app_id);

    inline constexpr u32 GetIdFromAddOnContentApplicationId(const u64 app_id) {
        return app_id & 0xFFF;
    }

    inline constexpr bool IsNacpEmpty(const NacpStruct &nacp) {
        return strlen(nacp.display_version) == 0;
    }

    std::string FindNacpName(const NacpStruct &nacp);
    std::string FindNacpAuthor(const NacpStruct &nacp);

    ApplicationIdMask GetApplicationIdMask(const u64 app_id);
    TicketFile ReadTicket(const std::string &path);
    void SaveTicket(fs::Explorer *exp, const std::string &path, const TicketFile tik_file);

}