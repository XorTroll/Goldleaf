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
        std::ifstream ifs(gsets.PathForResource("/Language/Strings/" + pdict));
        MainDictionary.DictLanguage = gsets.CustomLanguage;
        MainDictionary.Strings = JSON::parse(ifs);
        ifs.close();
        ifs.open(gsets.PathForResource("/Language/Errors/" + pdict));
        Errors.DictLanguage = gsets.CustomLanguage;
        Errors.Strings = JSON::parse(ifs);
        ifs.close();
    }
    
    pu::String GetDictionaryEntry(u32 Index)
    {
        return MainDictionary.Strings[Index].get<std::string>();
    }

    pu::String GetErrorEntry(u32 Index)
    {
        return Errors.Strings[Index].get<std::string>();
    }
}
