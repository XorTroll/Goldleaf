#include <gleaf/err/Error.hpp>

namespace gleaf::err
{
    Result Make(ErrorDescription Description)
    {
        return MAKERESULT(ErrorModule, static_cast<u32>(Description));
    }

    set::Dictionary GetDictionary()
    {
        Language slang = set::GetDictionary().DictLanguage;
        set::Dictionary dict = English;
        switch(slang)
        {
            case Language::English:
                dict = English;
                break;
            case Language::Spanish:
                // dict = Spanish;
                break;
        }
        return dict;
    }

    Error DetermineError(Result OSError)
    {
        Error err = { 0 };
        u32 vecidx = 0;
        u32 mod = R_MODULE(OSError);
        if(mod == 358) vecidx = 2;
        else switch(OSError)
        {
            case 0x202:
                vecidx = 1;
                break;
            case 0x234a02:
                vecidx = 3;
                break;
            case 0x236e02:
                vecidx = 4;
                break;
            default:
                vecidx = 0;
                break;
        }
        err.OSError = OSError;
        err.Description = GetDictionary().Strings[vecidx];
        err.DescIndex = vecidx;
        return err;
    }
}