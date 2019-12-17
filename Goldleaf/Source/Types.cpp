
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
#include <usb/usb_Detail.hpp>
#include <es/es_Service.hpp>

extern char** __system_argv;
extern bool gupdated;

namespace consts
{
    std::string Root = "switch/Goldleaf";
    std::string Log = Root + "/Goldleaf.log";
    std::string TempUpdatePath = Root + "/UpdateTemp.nro";
}

String Version::AsString()
{
    String txt = std::to_string(this->Major) + "." + std::to_string(this->Minor);
    if(this->Micro > 0) txt += "." + std::to_string(this->Micro);
    return txt;
}

Version Version::MakeVersion(u32 major, u32 minor, u32 micro)
{
    return { major, minor, (s32)micro };
}

Version Version::FromString(String StrVersion)
{
    String strv = StrVersion;
    Version v;
    memset(&v, 0, sizeof(v));
    size_t pos = 0;
    String token;
    u32 c = 0;
    String delimiter = ".";
    while((pos = strv.find(delimiter)) != String::npos)
    {
        token = strv.substr(0, pos);
        if(c == 0) v.Major = std::stoi(token);
        else if(c == 1) v.Minor = std::stoi(token);
        else if(c == 2) v.Micro = std::stoi(token);
        strv.erase(0, pos + delimiter.length());
        c++;
    }
    if(c == 0) v.Major = std::stoi(strv);
    else if(c == 1) v.Minor = std::stoi(strv);
    else if(c == 2) v.Micro = std::stoi(strv);
    return v;
}

bool Version::IsLower(Version Other)
{
    if(this->Major > Other.Major) return true;
    else if(this->Major == Other.Major)
    {
        if(this->Minor > Other.Minor) return true;
        else if(this->Minor == Other.Minor) if(this->Micro > Other.Micro) return true;
    }
    return false;
}

bool Version::IsHigher(Version Other)
{
    return !IsLower(Other);
}

bool Version::IsEqual(Version Other)
{
    return ((this->Major == Other.Major) && (this->Minor == Other.Minor) && (this->Micro == Other.Micro));
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

String GetVersion()
{
    return String(GOLDLEAF_VERSION);
}

bool IsAtmosphere()
{
    u64 tmpc = 0;
    return R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &tmpc));
}

u64 GetCurrentApplicationId()
{
    u64 appid = 0;
    svcGetInfo(&appid, InfoType_ProgramId, CUR_PROCESS_HANDLE, 0);
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
    sd->CreateDirectory(consts::Root);
    sd->CreateDirectory(consts::Root + "/meta");
    sd->CreateDirectory(consts::Root + "/title");
    sd->CreateDirectory(consts::Root + "/dump");
    sd->CreateDirectory(consts::Root + "/reports");
    sd->CreateDirectory(consts::Root + "/amiibocache");
    sd->CreateDirectory(consts::Root + "/userdata");
    sd->CreateDirectory(consts::Root + "/dump/temp");
    sd->CreateDirectory(consts::Root + "/dump/update");
    sd->CreateDirectory(consts::Root + "/dump/title");
}

void Close()
{
    if(GetLaunchMode() == LaunchMode::Application) libappletRequestHomeMenu();
    else exit(0);
}

Result Initialize()
{
    srand(time(NULL));
    EnsureDirectories();

    R_TRY(accountInitialize(AccountServiceType_Administrator));
    R_TRY(ncmInitialize());
    R_TRY(nsInitialize());
    R_TRY(es::Initialize());
    R_TRY(psmInitialize());
    R_TRY(setInitialize());
    R_TRY(setsysInitialize());
    R_TRY(usb::detail::Initialize());
    R_TRY(splInitialize());
    R_TRY(nifmInitialize(NifmServiceType_Admin));
    R_TRY(pdmqryInitialize());

    return 0;
}

void Exit()
{
    // If Goldleaf updated itself in this session...
    if(gupdated)
    {
        romfsExit();
        fs::DeleteFile(__system_argv[0]);
        fs::RenameFile(consts::TempUpdatePath, __system_argv[0]);
    }

    auto fsopsbuf = fs::GetFileSystemOperationsBuffer();
    operator delete[](fsopsbuf, std::align_val_t(0x1000));
    auto nsys = fs::GetNANDSystemExplorer();
    auto nsfe = fs::GetNANDSafeExplorer();
    auto nusr = fs::GetNANDUserExplorer();
    auto prif = fs::GetPRODINFOFExplorer();
    auto sdcd = fs::GetSdCardExplorer();
    delete nsys;
    delete nsfe;
    delete nusr;
    delete prif;
    delete sdcd;

    splExit();
    usb::detail::Exit();
    setsysExit();
    setExit();
    psmExit();
    es::Exit();
    nsExit();
    accountExit();
    ncmExit();
    nifmExit();
    pdmqryExit();
    Close();
}