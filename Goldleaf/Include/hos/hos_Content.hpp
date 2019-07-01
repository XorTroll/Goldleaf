
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <cstdio>
#include <cstring>
#include <switch.h>
#include <ncm/ncm_Types.hpp>

namespace hos
{
    std::string ContentIdAsString(const NcmNcaId &ContentId);
    NcmNcaId StringAsContentId(std::string ContentId);
}