
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

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <base_Common.hpp>
#include <ncm/ncm_Types.hpp> 
#include <ns/ns_Service.hpp>
#include <es/es_Service.hpp>

namespace hos {

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

    inline constexpr bool IsValidTicketSignature(u32 raw_val) {
        return (raw_val >= static_cast<u32>(TicketSignature::RSA_4096_SHA1)) && (raw_val <= static_cast<u32>(TicketSignature::ECDSA_SHA256));
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

        String GetFileName();
        String GetFullPath();
    };

    struct TitleContents {
        ContentId meta;
        ContentId program;
        ContentId data;
        ContentId control;
        ContentId html_document;
        ContentId legal_info;

        u64 GetTotalSize();
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
        bool IsBaseTitle() const;
        bool IsUpdate() const;
        bool IsAddOnContent() const;
        bool IsBaseOf(const Title &other) const;
        TitlePlayStats GetGlobalPlayStats() const;
        TitlePlayStats GetUserPlayStats(AccountUid user_id) const;
    };

    struct Ticket {
        es::RightsId rights_id;
        TicketType type;

        u64 GetApplicationId() const;
        u64 GetKeyGeneration() const;
        String ToString();
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
    };
    static_assert(sizeof(TicketData) == 0x180);

    constexpr u64 TicketSize = 0x2C0;

    inline constexpr u64 GetTicketSignatureSize(TicketSignature sig) {
        switch(sig) {
            case TicketSignature::RSA_4096_SHA1:
            case TicketSignature::RSA_4096_SHA256: {
                return sizeof(sig) + 0x200 + 0x3C;
            }
            case TicketSignature::RSA_2048_SHA1:
            case TicketSignature::RSA_2048_SHA256: {
                return sizeof(sig) + 0x100 + 0x3C;
            }
            case TicketSignature::ECDSA_SHA1:
            case TicketSignature::ECDSA_SHA256: {
                return sizeof(sig) + 0x3C + 0x40;
            }
            default: {
                return 0;
            }
        }
    }

    struct TicketFile {
        TicketSignature signature;
        TicketData data;

        String GetTitleKey() const;

        inline constexpr u64 GetFullSize() {
            return GetTicketSignatureSize(this->signature) + sizeof(this->data);
        }
    };

    constexpr u32 MaxTitleCount = 64000;

    std::string FormatApplicationId(u64 app_id);
    std::vector<Title> SearchTitles(NcmContentMetaType type, NcmStorageId storage_id);
    Title Locate(u64 app_id);
    bool ExistsTitle(NcmContentMetaType type, NcmStorageId storage_id, u64 app_id);
    std::vector<Ticket> GetAllTickets();
    Result RemoveTitle(const Title &title);
    Result RemoveTicket(const Ticket &tik);
    Result UpdateTitleVersion(const Title &title);
    std::string GetExportedIconPath(u64 app_id);
    String GetExportedNACPPath(u64 app_id);
    
    inline constexpr u64 GetBaseApplicationId(u64 app_id, NcmContentMetaType type) {
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

    inline constexpr u32 GetIdFromAddOnContentApplicationId(u64 app_id) {
        return app_id & 0xFFF;
    }

    inline constexpr bool IsNacpEmpty(const NacpStruct &nacp) {
        return strlen(nacp.display_version) == 0;
    }

    String FindNacpName(const NacpStruct &nacp);
    String FindNacpAuthor(const NacpStruct &nacp);

    ApplicationIdMask GetApplicationIdMask(u64 app_id);
    TicketFile ReadTicket(String path);

}