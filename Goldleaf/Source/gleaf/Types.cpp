#include <gleaf/Types.hpp>

namespace gleaf
{
    bool InstallerResult::IsSuccess()
    {
        return (this->Type == InstallerError::Success);
    }

    std::string Version::AsString()
    {
        std::string txt = std::to_string(this->Major) + "." + std::to_string(this->Minor);
        if(this->BugFix > 0)
        {
            txt += "." + std::to_string(this->BugFix);
        }
        return txt;
    }

    Version Version::FromString(std::string StrVersion)
    {
        std::string strv = StrVersion;
        Version v;
        memset(&v, 0, sizeof(v));
        size_t pos = 0;
        std::string token;
        u32 c = 0;
        std::string delimiter = ".";
        while((pos = strv.find(delimiter)) != std::string::npos)
        {
            token = strv.substr(0, pos);
            if(c == 0) v.Major = std::stoi(token);
            else if(c == 1) v.Minor = std::stoi(token);
            else if(c == 2) v.BugFix = std::stoi(token);
            strv.erase(0, pos + delimiter.length());
            c++;
        }
        if(c == 0) v.Major = std::stoi(strv);
        else if(c == 1) v.Minor = std::stoi(strv);
        else if(c == 2) v.BugFix = std::stoi(strv);
        return v;
    }

    bool Version::IsLower(Version Other)
    {
        bool low = false;
        if(this->Major > Other.Major) low = true;
        else if(this->Minor > Other.Minor) low = true;
        else if(this->BugFix > Other.BugFix) low = true;
        return low;
    }

    bool Version::IsHigher(Version Other)
    {
        return !IsLower(Other);
    }

    bool Version::IsEqual(Version Other)
    {
        bool eq = false;
        if(this->Major > Other.Major) eq = true;
        else if(this->Minor > Other.Minor) eq = true;
        else if(this->BugFix > Other.BugFix) eq = true;
        return ((this->Major == Other.Major) && (this->Minor == Other.Minor) && (this->BugFix == Other.BugFix));
    }
}