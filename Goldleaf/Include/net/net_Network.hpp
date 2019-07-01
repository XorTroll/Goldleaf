
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <Types.hpp>
#include <curl/curl.h>

namespace net
{
    std::string RetrieveContent(std::string URL, std::string MIMEType = "");
    void RetrieveToFile(std::string URL, std::string Path, std::function<void(double Done, double Total)> Callback);
    bool CheckVersionDiff();
    bool HasConnection();
}