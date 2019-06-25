
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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