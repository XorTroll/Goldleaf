
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from devkitPro's switch tool's source (build_pfs0)

*/

#pragma once
#include <string>
#include <functional>
#include <nsp/nsp_Types.hpp>

namespace nsp
{
    int Build(pu::String ContentsDir, pu::String Out, std::function<void(u8 Percentage)> Callback);
}