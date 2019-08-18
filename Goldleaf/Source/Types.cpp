#include <Types.hpp>
#include <fs/fs_Explorer.hpp>

pu::String Version::AsString()
{
    pu::String txt = std::to_string(this->Major) + "." + std::to_string(this->Minor);
    if(this->BugFix > 0)
    {
        txt += "." + std::to_string(this->BugFix);
    }
    return txt;
}

Version Version::FromString(pu::String StrVersion)
{
    pu::String strv = StrVersion;
    Version v;
    memset(&v, 0, sizeof(v));
    size_t pos = 0;
    pu::String token;
    u32 c = 0;
    pu::String delimiter = ".";
    while((pos = strv.find(delimiter)) != pu::String::npos)
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

pu::String GetVersion()
{
    return pu::String(GOLDLEAF_VERSION);
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

u64 GetCurrentApplicationId()
{
    u64 appid = 0;
    svcGetInfo(&appid, InfoType_TitleId, CUR_PROCESS_HANDLE, 0);
    return appid;
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
    fs::CreateDirectory("sdmc:/" + GoldleafDir);
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/meta");
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/title");
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/dump");
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/userdata");
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/dump/temp");
    fs::CreateDirectory("sdmc:/" + GoldleafDir + "/dump/out");
}