
#pragma once
#include <vector>
#include <switch.h>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

namespace gleaf::theme
{
    enum class CFWType
    {
        Atmosphere,
        ReiNX,
        SXOS,
    };

    std::vector<CFWType> GetCFWsOnSdCard();
    bool IsCFWOnSdCard(CFWType Type);
}