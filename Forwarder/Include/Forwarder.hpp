#include <string>
#include <vector>
#include <switch.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include "hbl.h"

static std::vector<std::string> GoldleafPaths =
{
    "switch/Goldleaf.nro",
    "switch/Goldleaf/Goldleaf.nro",
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

inline void ForwarderProcess(std::string Arg)
{
    std::string gnro = GetGoldleaf();
    if(gnro.empty()) FatalError("find Goldleaf NRO", "Place it on '/switch' folder.");
    targetNro(gnro.c_str(), (gnro + " " + Arg).c_str());
}