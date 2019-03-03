#include <gleaf/err/Error.hpp>
#include <errno.h>

namespace gleaf::err
{
    Result Make(ErrorDescription Description)
    {
        return MAKERESULT(ErrorModule, static_cast<u32>(Description));
    }

    Result MakeErrno(int Val)
    {
        if(Val >= 0) return 0;
        return MAKERESULT(ErrnoErrorModule, (u32)errno);
    }

    set::Dictionary GetDictionary()
    {
        Language slang = set::GetDictionary().DictLanguage;
        set::Dictionary dict = English;
        switch(slang)
        {
            default:
                dict = English;
                break;
        }
        return dict;
    }

    Error DetermineError(Result OSError)
    {
        Error err = { 0 };
        u32 vecidx = 0;
        u32 modidx = 0;
        u32 mod = R_MODULE(OSError);
        if(mod == 358) vecidx = 5;
        else if (mod == 356)
        {
            ErrorDescription desc = static_cast<ErrorDescription>(R_DESCRIPTION(OSError));
            switch(desc)
            {
                case ErrorDescription::NotEnoughSize:
                    vecidx = 6;
                    break;
                case ErrorDescription::MetaNotFound:
                    vecidx = 7;
                    break;
                case ErrorDescription::CNMTNotFound:
                    vecidx = 8;
                    break;
                case ErrorDescription::TitleAlreadyInstalled:
                    vecidx = 9;
                    break;
                case ErrorDescription::BadGLUCCommand:
                    vecidx = 10;
                    break;
                case ErrorDescription::FileDirectoryAlreadyPresent:
                    vecidx = 11;
                    break;
            }
        }
        else switch(OSError)
        {
            case 0x202:
                vecidx = 1;
                break;
            case 0x234a02:
                vecidx = 2;
                break;
            case 0x236e02:
                vecidx = 3;
                break;
            default:
                vecidx = 0;
                break;
        }
        switch(mod)
        {
            case 128:
                modidx = 0;
                break;
            case 124:
                modidx = 1;
                break;
            case 5:
                modidx = 2;
                break;
            case 8:
                modidx = 3;
                break;
            case 15:
                modidx = 4;
                break;
            case 21:
                modidx = 5;
                break;
            case 16:
                modidx = 6;
                break;
            case 145:
                modidx = 7;
                break;
            case 136:
                modidx = 8;
                break;
            case 135:
                modidx = 9;
                break;
            case 26:
                modidx = 10;
                break;
            case 105:
                modidx = 11;
                break;
            case 140:
                modidx = 12;
                break;
            case 2:
                modidx = 13;
                break;
            case 356:
                modidx = 14;
                break;
            case 358:
                modidx = 15;
                break;
        }
        err.OSError = OSError;
        err.Module = Modules[modidx];
        err.Description = GetDictionary().Strings[vecidx];
        err.DescIndex = vecidx;
        return err;
    }
}