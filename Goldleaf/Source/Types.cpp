
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <fs/fs_FileSystem.hpp>
#include <usb/usb_Detail.hpp>
#include <es/es_Service.hpp>

extern char **__system_argv;
extern bool global_app_updated;

String LowerCaseString(String str)
{
    String ret;
    auto u8str = str.AsUTF8();
    std::transform(u8str.begin(), u8str.end(), u8str.begin(), tolower);
    return u8str;
}

std::string LanguageToString(Language lang)
{
    std::string langstr = "en"; // Default
    switch(lang)
    {
        case Language::English:
            return "en";
        case Language::Spanish:
            return "es";
        case Language::German:
            return "de";
        case Language::French:
            return "fr";
        case Language::Italian:
            return "it";
        case Language::Dutch:
            return "nl";
        default:
            break;
    }
    return langstr;
}

Language StringToLanguage(std::string str)
{
    auto lang = Language::English;
    if(str == "en") lang = Language::English;
    else if(str == "es") lang = Language::Spanish;
    else if(str == "de") lang = Language::German;
    else if(str == "fr") lang = Language::French;
    else if(str == "it") lang = Language::Italian;
    else if(str == "nl") lang = Language::Dutch;
    return lang;
}

String Version::AsString()
{
    String txt = std::to_string(this->Major) + "." + std::to_string(this->Minor);
    if(this->Micro > 0) txt += "." + std::to_string(this->Micro);
    return txt;
}

Version Version::FromString(String StrVersion)
{
    auto strv = StrVersion;
    Version v = {};
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
        else if(this->Minor == Other.Minor)
        {
            if(this->Micro > Other.Micro) return true;
        }
    }
    return false;
}

bool Version::IsHigher(Version Other)
{
    return !this->IsLower(Other) && !this->IsEqual(Other);
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

u64 GetCurrentApplicationId()
{
    u64 appid = 0;
    svcGetInfo(&appid, InfoType_ProgramId, CUR_PROCESS_HANDLE, 0);
    return appid;
}

u32 RandomFromRange(u32 Min, u32 Max)
{
    u32 diff = Max - Min;
    u32 random_val;
    randomGet(&random_val, sizeof(random_val));
    random_val %= (diff + 1);
    return random_val + Min;
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
    srand(time(nullptr));
    EnsureDirectories();

    R_TRY(accountInitialize(AccountServiceType_Administrator));
    R_TRY(ncmInitialize());
    R_TRY(nsInitialize());
    R_TRY(es::Initialize());
    R_TRY(psmInitialize());
    R_TRY(setInitialize());
    R_TRY(setsysInitialize());
    R_TRY(usb::detail::Initialize());
    R_TRY(nifmInitialize(NifmServiceType_Admin));
    R_TRY(pdmqryInitialize());

    if(drive::IsFspUsbAccessible()) R_TRY(drive::Initialize());

    return 0;
}

void Exit()
{
    auto nsys = fs::GetNANDSystemExplorer();
    auto nsfe = fs::GetNANDSafeExplorer();
    auto nusr = fs::GetNANDUserExplorer();
    auto prif = fs::GetPRODINFOFExplorer();
    auto sdcd = fs::GetSdCardExplorer();

    // If we updated ourselves in this session...
    if(global_app_updated)
    {
        romfsExit();

        const auto cur_nro_file = __system_argv[0];
        sdcd->DeleteFile(cur_nro_file);
        sdcd->RenameFile(consts::TempUpdatedNro, cur_nro_file);
    }

    auto work_buf = fs::GetWorkBuffer();
    operator delete[](work_buf, std::align_val_t(0x1000));
    
    delete nsys;
    delete nsfe;
    delete nusr;
    delete prif;
    delete sdcd;

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