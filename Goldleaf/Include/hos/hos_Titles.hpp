
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <Types.hpp>
#include <ncm/ncm_Service.hpp>
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

    enum class ApplicationIdMask
    {
        Official,
        Homebrew,
        Invalid,
    };

    struct ContentId
    {
        ncm::ContentType Type;
        NcmNcaId NCAId;
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
        ncm::ContentMetaType Type;
        u32 Version;
        NcmMetaRecord Record;
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
        TitlePlayStats GetUserPlayStats(u128 UserId);
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
    std::vector<Title> SearchTitles(ncm::ContentMetaType Type, Storage Location);
    Title Locate(u64 ApplicationId);
    bool ExistsTitle(ncm::ContentMetaType Type, Storage Location, u64 ApplicationId);
    std::vector<Ticket> GetAllTickets();
    Result RemoveTitle(Title &ToRemove);
    Result RemoveTicket(Ticket &ToRemove);
    std::string GetExportedIconPath(u64 ApplicationId);
    pu::String GetExportedNACPPath(u64 ApplicationId);
    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type);
    ApplicationIdMask IsValidApplicationId(u64 ApplicationId);
    TicketData ReadTicket(pu::String Path);
    pu::String GetNACPName(NacpStruct *NACP);
    pu::String GetNACPAuthor(NacpStruct *NACP);
    pu::String GetNACPVersion(NacpStruct *NACP);
}