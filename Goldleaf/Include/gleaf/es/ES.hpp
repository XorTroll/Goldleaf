
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <cstring>
#include <switch/arm/atomics.h>
#include <tuple>
#include <gleaf/es/RightsId.hpp>

namespace gleaf::es
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result ImportTicket(void const *Ticket, size_t TicketSize, void const *Cert, size_t CertSize);
    Result DeleteTicket(const RightsId *RId, size_t RIdSize);
    std::tuple<Result, u8*, size_t> GetTitleKey(const RightsId *RId);
    std::tuple<Result, u8> CountCommonTicket();
    std::tuple<Result, u8> CountPersonalizedTicket();
    std::tuple<Result, u32, RightsId*> ListCommonTicket(size_t Size);
    std::tuple<Result, u32, RightsId*> ListPersonalizedTicket(size_t Size);
    std::tuple<Result, u64, void*, size_t> GetCommonTicketData(const RightsId *RId);
}