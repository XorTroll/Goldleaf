
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <string>
#include <vector>
#include <switch.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include "hbl.h"

static std::vector<std::string> GoldleafPaths =
{
    "switch/Goldleaf/Goldleaf.nro",
    "switch/Goldleaf.nro",
};

inline std::string GetGoldleaf()
{
    std::string gp;
    for(u32 i = 0; i < GoldleafPaths.size(); i++)
    {
        std::string gnro = "sdmc:/" + GoldleafPaths[i];
        std::ifstream ifs(gnro);
        bool iok = ifs.good();
        ifs.close();
        if(iok)
        {
            gp = gnro;
            break;
        }
    }
    return gp;
}

inline void FatalError(std::string Err, std::string MoreInfo)
{
    consoleInit(NULL);
    std::cout << std::endl << "FATAL - An error ocurred attempting to " << Err << "." << std::endl << MoreInfo;
    consoleUpdate(NULL);
    while(true);
}