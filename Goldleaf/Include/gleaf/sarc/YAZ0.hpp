
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <iostream>
#include <vector>
#include <gleaf/Types.hpp>

namespace gleaf::sarc 
{
    namespace YAZ0
    {
        std::vector<u8> Decompress(std::vector<u8> &Data);
        std::vector<u8> Compress(std::vector<u8> &Data, int level = 3, int reserved1 = 0, int reserved2 = 0);
    }
}