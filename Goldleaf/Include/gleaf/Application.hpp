
#pragma once
#include <gleaf/es.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    void Initialize();
    void Finalize();
    bool IsApplication();
    std::vector<std::string> GetSdCardCFWs();
    std::vector<std::string> GetSdCardCFWNames();
    std::string GetCFWName(std::string Path);
}