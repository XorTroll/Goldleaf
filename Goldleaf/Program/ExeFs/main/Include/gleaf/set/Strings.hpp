
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/set/json.hpp>
#include <gleaf/set/Settings.hpp>

namespace gleaf::set
{
    using json = nlohmann::json;

    struct Dictionary
    {
        Language DictLanguage;
        json Strings;
    };

    static Dictionary MainDictionary;
    
    void Initialize();
    std::string GetDictionaryEntry(u32 Index);
}
