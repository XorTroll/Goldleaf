#include <gleaf/set/Strings.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::set
{
    std::string GetDictionaryEntry(u32 Index)
    {
        Language slang = gsets.CustomLanguage;
        Dictionary dict;
        switch(slang)
        {
            case Language::English:
                dict = English;
                break;
            case Language::Spanish:
                dict = Spanish;
                break;
        }
        return dict.Strings[Index];
    }
}