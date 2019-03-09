#include <gleaf/set/Strings.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::set
{
    Dictionary GetDictionary()
    {
        Language slang = gsets.CustomLanguage;
        Dictionary dict = English;
        switch(slang)
        {
            case Language::English:
                dict = English;
                break;
            case Language::Spanish:
                dict = Spanish;
                break;
            case Language::German:
                dict = German;
                break;
            case Language::French:
                dict = French;
                break;
            case Language::Italian:
                dict = Italian;
                break;
        }
        return dict;
    }
    
    std::string GetDictionaryEntry(u32 Index)
    {
        return GetDictionary().Strings[Index];
    }
}
