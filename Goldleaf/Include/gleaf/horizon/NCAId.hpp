
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <cstdio>
#include <cstring>
#include <switch.h>
#include <gleaf/ncm/Content.hpp>

namespace gleaf::horizon
{
    std::string GetStringFromNCAId(const NcmNcaId &NCAId);
    NcmNcaId GetNCAIdFromString(std::string NCAId);
}