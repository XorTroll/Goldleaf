
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/es.hpp>
#include <gleaf/fs.hpp>
#include <gleaf/horizon.hpp>
#include <gleaf/ncm.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/nsp.hpp>
#include <gleaf/ui.hpp>
#include <gleaf/usb.hpp>

namespace gleaf
{
    void Initialize();
    void Finalize();
    bool IsApplication();
}