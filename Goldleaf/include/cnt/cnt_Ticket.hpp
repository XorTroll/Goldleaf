
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
#include <fs/fs_FspExplorers.hpp>
#include <cnt/cnt_Content.hpp>

namespace cnt {

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
        HMAC_SHA1_160 = 0x10006
    };

    NX_CONSTEXPR bool IsValidTicketSignature(const TicketSignature sig) {
        return (sig >= TicketSignature::RSA_4096_SHA1) && (sig <= TicketSignature::ECDSA_SHA256);
    }

    struct Ticket {
        EsRightsId rights_id;
        TicketType type;

        inline std::optional<std::reference_wrapper<Application>> IsUsed() const {
            return ExistsApplicationContent(esGetRightsIdApplicationId(&this->rights_id));
        }
        
        std::string ToString() const;
    };

    enum class TicketFlags : u8 {
        PreInstalled = BIT(0),
        Shared = BIT(1),
        AllContents = BIT(2),
        DeviceLinkIndependent = BIT(3),
        Temporary = BIT(4)
    };
    GLEAF_DEFINE_FLAG_ENUM(TicketFlags, u8);

    struct TicketData {
        u8 issuer[0x40];
        u8 title_key_block[0x100];
        u8 ticket_version;
        u8 title_key_type;
        u8 unk_1[0x2];
        u8 license_type;
        u8 master_key_gen;
        TicketFlags flags;
        u8 unk_2[0x8];
        u8 ticket_id[0x8];
        u8 device_id[0x8];
        EsRightsId rights_id;
        u8 account_id[0x4];
        u8 unk_3[0xC];

        std::string GetTitleKey() const;
    };
    static_assert(sizeof(TicketData) == 0x180);

    NX_CONSTEXPR size_t GetTicketSignatureDataSize(const TicketSignature sig) {
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
            case TicketSignature::HMAC_SHA1_160: {
                return 0x14 + 0x28;
            }
            default: {
                return 0;
            }
        }
    }
    
    NX_CONSTEXPR size_t GetTicketSignatureSize(const TicketSignature sig) {
        return sizeof(sig) + GetTicketSignatureDataSize(sig);
    }

    struct TicketFile {
        TicketSignature signature;
        u8 signature_data[0x300];
        TicketData data;

        inline constexpr size_t GetFullSize() {
            return GetTicketSignatureSize(this->signature) + sizeof(this->data);
        }
    };

    void InitializeTickets();
    void FinalizeTickets();
    void NotifyTicketsChanged();
    std::vector<Ticket> &GetTickets();

    bool TryFindApplicationTicket(const u64 app_id, Ticket &out_tik);
    Result RemoveTicket(const Ticket &tik);
    TicketFile ReadTicket(const std::string &path);
    void SaveTicket(fs::Explorer *exp, const std::string &path, const TicketFile tik_file);
    std::string GetKeyGenerationRange(const u8 key_gen);

}
