
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <gleaf/Types.hpp>
#include <gleaf/ncm.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/es.hpp>

namespace gleaf::horizon
{
    enum class TicketType
    {
        Common,
        Personalized,
    };

    enum class TicketSignature
    {
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

        std::string GetFileName();
        std::string GetFullPath();
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
        std::string GetFormattedTotalSize();
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
    };

    struct Ticket
    {
        es::RightsId RId;
        TicketType Type;

        u64 GetApplicationId();
        u64 GetKeyGeneration();
        std::string ToString();
    };

    struct TicketData
    {
        TicketSignature Signature;
        std::string TitleKey;
        u8 KeyGeneration;
    };

    std::string FormatApplicationId(u64 ApplicationId);
    std::vector<Title> SearchTitles(ncm::ContentMetaType Type, Storage Location);
    Title Locate(u64 ApplicationId);
    bool ExistsTitle(ncm::ContentMetaType Type, Storage Location, u64 ApplicationId);
    std::vector<Ticket> GetAllTickets();
    Result RemoveTitle(Title &ToRemove);
    std::string GetExportedIconPath(u64 ApplicationId);
    std::string GetExportedNACPPath(u64 ApplicationId);
    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type);
    ApplicationIdMask IsValidApplicationId(u64 ApplicationId);
    TicketData ReadTicket(std::string Path);
    std::string GetNACPName(NacpStruct *NACP);
    std::string GetNACPAuthor(NacpStruct *NACP);
    std::string GetNACPVersion(NacpStruct *NACP);
}