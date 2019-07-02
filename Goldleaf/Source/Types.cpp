#include <Types.hpp>
#include <fs/fs_Explorer.hpp>

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
    return ((this->Major == Other.Major) && (this->Minor == Other.Minor) && (this->BugFix == Other.BugFix));
}

ExecutableMode GetExecutableMode()
{
    return envIsNso() ? ExecutableMode::NSO : ExecutableMode::NRO;
}

LaunchMode GetLaunchMode()
{
    LaunchMode mode = LaunchMode::Unknown;
    AppletType type = appletGetAppletType();
    switch(type)
    {
        case AppletType_SystemApplication:
        case AppletType_Application:
            mode = LaunchMode::Application;
            break;
        // Shall I add other applet types? Don't think this will run over qlaunch or overlay...
        case AppletType_LibraryApplet:
            mode = LaunchMode::Applet;
        default:
            break;
    }
    return mode;
}

std::string GetVersion()
{
    return std::string(GOLDLEAF_VERSION);
}

u64 GetApplicationId()
{
    return GOLDLEAF_APPID;
}

bool IsAtmosphere()
{
    u64 tmpc = 0;
    return R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &tmpc));
}

bool IsReiNX()
{
    Handle tmph = 0;
    Result rc = smRegisterService(&tmph, "rnx", false, 1);
    if(R_FAILED(rc)) return true;
    smUnregisterService("rnx");
    return false;
}

bool IsSXOS()
{
    Handle tmph = 0;
    Result rc = smRegisterService(&tmph, "tx", false, 1);
    if(R_FAILED(rc)) return true;
    smUnregisterService("tx");
    return false;
}

u32 RandomFromRange(u32 Min, u32 Max)
{
    u32 diff = Max - Min;
    u32 rval = rand() % (diff + 1);
    return rval + Min;
}

void EnsureDirectories()
{
    fs::Explorer *nsys = fs::GetNANDSystemExplorer();
    nsys->DeleteDirectory("Contents/temp");
    nsys->CreateDirectory("Contents/temp");
    fs::CreateDirectory("sdmc:/goldleaf");
    fs::CreateDirectory("sdmc:/goldleaf/meta");
    fs::CreateDirectory("sdmc:/goldleaf/title");
    fs::CreateDirectory("sdmc:/goldleaf/dump");
    fs::CreateDirectory("sdmc:/goldleaf/userdata");
    fs::CreateDirectory("sdmc:/goldleaf/dump/temp");
    fs::CreateDirectory("sdmc:/goldleaf/dump/out");
}