
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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
#include <Types.hpp>
#include <switch/services/ncm.h>
#include <ns/ns_Service.hpp>
#include <es/es_Service.hpp>

namespace hos
{
    enum class TicketType
    {
        Common,
        Personalized,
    };

    enum class TicketSignature
    {
        Invalid,
        RSA_4096_SHA1 = 0x10000,
        RSA_2048_SHA1 = 0x10001,
        ECDSA_SHA1 = 0x10002,
        RSA_4096_SHA256 = 0x10003,
        RSA_2048_SHA256 = 0x10004,
        ECDSA_SHA256 = 0x10005,
    };

    inline constexpr bool IsValidTicketSignature(u32 RawValue)
    {
        return (RawValue >= static_cast<u32>(TicketSignature::RSA_4096_SHA1)) && (RawValue <= static_cast<u32>(TicketSignature::ECDSA_SHA256));
    }

    enum class ApplicationIdMask
    {
        Official,
        Homebrew,
        Invalid,
    };

    struct ContentId
    {
        NcmContentType Type;
        NcmContentId NCAId;
        Storage Location;
        bool Empty;
        u64 Size;

        pu::String GetFileName();
        pu::String GetFullPath();
    };

    struct TitleContents
    {
        ContentId Meta;
        ContentId Program;
        ContentId Data;
        ContentId Control;
        ContentId HtmlDocument;
        ContentId LegalInfo;

        u64 GetTotalSize();
        pu::String GetFormattedTotalSize();
    };

    struct TitlePlayStats
    {
        u64 TotalPlaySeconds;
        u64 SecondsFromLastLaunched;
        u64 SecondsFromFirstLaunched;
    };

    struct Title
    {
        u64 ApplicationId;
        NcmContentMetaType Type;
        u32 Version;
        NcmContentMetaKey Record;
        Storage Location;
        
        NacpStruct *TryGetNACP();
        u8 *TryGetIcon();
        bool DumpControlData();
        TitleContents GetContents();
        bool IsBaseTitle();
        bool IsUpdate();
        bool IsDLC();
        bool CheckBase(Title &Other);
        TitlePlayStats GetGlobalPlayStats();
        TitlePlayStats GetUserPlayStats(AccountUid UserId);
    };

    struct Ticket
    {
        es::RightsId RId;
        TicketType Type;

        u64 GetApplicationId();
        u64 GetKeyGeneration();
        pu::String ToString();
    };

    struct TicketData
    {
        TicketSignature Signature;
        std::string TitleKey;
        u8 KeyGeneration;
    };

    static constexpr u32 MaxTitleCount = 64000;

    std::string FormatApplicationId(u64 ApplicationId);
    std::vector<Title> SearchTitles(NcmContentMetaType Type, Storage Location);
    Title Locate(u64 ApplicationId);
    bool ExistsTitle(NcmContentMetaType Type, Storage Location, u64 ApplicationId);
    std::vector<Ticket> GetAllTickets();
    Result RemoveTitle(Title &ToRemove);
    Result RemoveTicket(Ticket &ToRemove);
    std::string GetExportedIconPath(u64 ApplicationId);
    pu::String GetExportedNACPPath(u64 ApplicationId);
    u64 GetBaseApplicationId(u64 ApplicationId, NcmContentMetaType Type);
    u32 GetIdFromDLCApplicationId(u64 ApplicationId);
    ApplicationIdMask IsValidApplicationId(u64 ApplicationId);
    TicketData ReadTicket(pu::String Path);
    pu::String GetNACPName(NacpStruct *NACP);
    pu::String GetNACPAuthor(NacpStruct *NACP);
    pu::String GetNACPVersion(NacpStruct *NACP);
}