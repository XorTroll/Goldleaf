#include <set/set_Strings.hpp>

extern set::Settings gsets;

namespace set
{
    void Initialize()
    {
        std::string pdict;
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
        }
        pdict += ".json";
        std::ifstream ifs(gsets.PathForResource("/Language/Strings/" + pdict));
        MainDictionary.DictLanguage = gsets.CustomLanguage;
        MainDictionary.Strings = JSON::parse(ifs);
        ifs.close();
        ifs.open(gsets.PathForResource("/Language/Errors/" + pdict));
        Errors.DictLanguage = gsets.CustomLanguage;
        Errors.Strings = JSON::parse(ifs);
        ifs.close();
    }
    
    std::string GetDictionaryEntry(u32 Index)
    {
        return MainDictionary.Strings[Index].get<std::string>();
    }

    std::string GetErrorEntry(u32 Index)
    {
        return Errors.Strings[Index].get<std::string>();
    }
}
