
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <es/es_Types.hpp>

namespace es
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result ImportTicket(void const *Ticket, size_t TicketSize, void const *Cert, size_t CertSize);
    Result DeleteTicket(const RightsId *RId, size_t RIdSize);
    Result GetTitleKey(const RightsId *RId, u8 *out_Key, size_t out_KeySize);
    Result CountCommonTicket(u8 *out_Count);
    Result CountPersonalizedTicket(u8 *out_Count);
    Result ListCommonTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize);
    Result ListPersonalizedTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize);
    Result GetCommonTicketData(const RightsId *RId, void *out_Data, size_t out_DataSize, u64 *out_Unk);
}