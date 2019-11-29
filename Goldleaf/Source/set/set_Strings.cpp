
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <set/set_Strings.hpp>
#include <fs/fs_Common.hpp>

extern set::Settings gsets;

namespace set
{
    void Initialize()
    {
        std::string pdict = "en"; // By default
        switch(gsets.CustomLanguage)
        {
            case Language::English:
                pdict = "en";
                break;
            case Language::Spanish:
                pdict = "es";
                break;
            case Language::German:
                pdict = "de";
                break;
            case Language::French:
                pdict = "fr";
                break;  
            case Language::Italian:
                pdict = "it";
                break;
            case Language::Dutch:
                pdict = "nl";
                break;
        }
        pdict += ".json";
        MainDictionary.DictLanguage = gsets.CustomLanguage;
        std::ifstream ifs(gsets.PathForResource("/Language/Strings/" + pdict));
        if(ifs.good())
        {
            try { MainDictionary.Strings = JSON::parse(ifs); } catch(std::exception&) {}
            ifs.close();
        }
        Errors.DictLanguage = gsets.CustomLanguage;
        ifs.open(gsets.PathForResource("/Language/Errors/" + pdict));
        if(ifs.good())
        {
            try { Errors.Strings = JSON::parse(ifs); } catch(std::exception&) {}
            ifs.close();
        }
    }
    
    pu::String GetDictionaryEntry(u32 Index)
    {
        if(Index >= MainDictionary.Strings.size()) return "<invalid entry>";
        return MainDictionary.Strings[Index].get<std::string>();
    }

    pu::String GetErrorEntry(u32 Index)
    {
        if(Index >= Errors.Strings.size()) return "<invalid error entry>";
        return Errors.Strings[Index].get<std::string>();
    }
}
