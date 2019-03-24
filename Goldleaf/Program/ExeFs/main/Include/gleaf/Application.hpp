
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/es.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    void Initialize();
    void Finalize();
    void EnsureDirectories();
    RunMode GetRunMode();
    bool IsNRO();
    bool IsInstalledTitle();
    bool IsQlaunch();
    std::vector<std::string> GetSdCardCFWs();
    std::vector<std::string> GetSdCardCFWNames();
    std::string GetCFWName(std::string Path);
    bool HasKeyFile();
    bool IsAtmosphere();
    bool IsReiNX();
    bool IsSXOS();
    u32 RandomFromRange(u32 Min, u32 Max);
    std::string GetKeyFilePath();
    std::vector<std::string> GetKeyFilePossibleNames();
}