
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <set/set_Settings.hpp>

namespace set
{
    struct Dictionary
    {
        Language DictLanguage;
        JSON Strings;
    };

    static Dictionary MainDictionary;
    static Dictionary Errors;
    
    void Initialize();
    std::string GetDictionaryEntry(u32 Index);
    std::string GetErrorEntry(u32 Index);
}
