
#pragma once
#include <gleaf/es.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    void Initialize();
    void Finalize();
    bool IsApplication();

    static const std::vector<std::string> CFWList =
    {
        "Atmosphère",
        "ReiNX",
        "SX OS",
    };

    static const std::vector<std::string> CFWDirectories =
    {
        "atmosphere",
        "reinx",
        "sxos",
    };
}