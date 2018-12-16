
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

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

    std::string FormatApplicationId(u64 ApplicationId);
    std::vector<Title> GetAllSystemTitles();
    std::vector<Ticket> GetAllSystemTickets();
    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type);
}