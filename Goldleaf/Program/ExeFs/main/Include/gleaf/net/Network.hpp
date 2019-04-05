
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Types.hpp>
#include <curl/curl.h>

namespace gleaf::net
{
    std::string RetrieveContent(std::string URL, std::string MIMEType = "");
    void RetrieveToFile(std::string URL, std::string Path, std::function<void(u8)> Callback);
    bool CheckVersionDiff();
    bool HasConnection();
}