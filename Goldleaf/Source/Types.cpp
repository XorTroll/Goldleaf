
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
    SmServiceName rnxSrv = { "rnx" };
    Handle tmph = 0;
    Result rc = smRegisterService(&tmph, rnxSrv, false, 1);
    if(R_FAILED(rc)) return true;
    smUnregisterService(rnxSrv);
    return false;
}

bool IsSXOS()
{
    SmServiceName txSrv = { "tx" };
    Handle tmph = 0;
    Result rc = smRegisterService(&tmph, txSrv, false, 1);
    if(R_FAILED(rc)) return true;
    smUnregisterService(txSrv);
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
    auto nsys = fs::GetNANDSystemExplorer();
    auto sd = fs::GetSdCardExplorer();
    nsys->DeleteDirectory("Contents/temp");
    nsys->CreateDirectory("Contents/temp");
    sd->CreateDirectory(GoldleafDir);
    sd->CreateDirectory(GoldleafDir + "/meta");
    sd->CreateDirectory(GoldleafDir + "/title");
    sd->CreateDirectory(GoldleafDir + "/dump");
    sd->CreateDirectory(GoldleafDir + "/userdata");
    sd->CreateDirectory(GoldleafDir + "/dump/temp");
    sd->CreateDirectory(GoldleafDir + "/dump/update");
    sd->CreateDirectory(GoldleafDir + "/dump/title");
}