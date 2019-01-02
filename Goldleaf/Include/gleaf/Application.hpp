
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
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
    bool IsApplication();
    std::vector<std::string> GetSdCardCFWs();
    std::vector<std::string> GetSdCardCFWNames();
    std::string GetCFWName(std::string Path);
    bool HasKeyFile();
    std::string GetKeyFilePath();
    std::vector<std::string> GetKeyFilePossibleNames();
}