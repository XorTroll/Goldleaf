
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <iostream>
#include <switch.h>

namespace gleaf
{
    enum class Destination
    {
        NAND,
        SdCard,
    };

    enum class Storage
    {
        SdCard,
        NAND,
        GameCart,
    };
}