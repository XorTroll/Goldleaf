
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <gleaf/Types.hpp>
#include <gleaf/ncm.hpp>
#include <gleaf/es.hpp>

namespace gleaf::horizon
{
    enum class TicketType
    {
        Common,
        Personalized,
    };

    // Thanks Switchbrew
    enum class TicketSignature
    {
        RSA_4096_SHA1 = 0x10000,
        RSA_2048_SHA1 = 0x10001,
        ECDSA_SHA1 = 0x10002,
        RSA_4096_SHA256 = 0x10003,
        RSA_2048_SHA256 = 0x10004,
        ECDSA_SHA256 = 0x10005,
    };

    struct Title
    {
        u64 ApplicationId;
        Storage Location;
        std::string Name;
        std::string Author;
        std::string Version;
        NacpStruct NACP;

        std::string GetExportedIconPath();
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

    struct PartitionFsHeader
    {
        u32 Magic;
        u32 FileCount;
        u32 StringTableSize;
        u32 Reserved;
    } PACKED;

    struct PartitionFsFileEntry
    {
        u64 Offset;
        u64 Size;
        u64 StringTableOffset;
        u32 Pad;
    } PACKED;

    std::string FormatApplicationId(u64 ApplicationId);
    std::vector<Title> GetAllSystemTitles();
    std::vector<Ticket> GetAllSystemTickets();
    std::string GetTitleNCAPath(u64 ApplicationId);
    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type);
    TicketData ReadTicket(std::string Path);
}