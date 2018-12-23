
#pragma once
#include <gleaf/es.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    void Initialize();
    void Finalize();
    bool IsApplication();

    static const std::string Version = "0.1";
}