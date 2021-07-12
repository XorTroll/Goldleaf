
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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
#include <amssu/amssu_Service.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;

extern char **__system_argv;
extern bool g_UpdatedNeedsRename;

String LowerCaseString(String str) {
    String ret;
    auto u8_str = str.AsUTF8();
    std::transform(u8_str.begin(), u8_str.end(), u8_str.begin(), tolower);
    return u8_str;
}

std::string LanguageToString(Language lang) {
    switch(lang) {
        case Language::Spanish: {
            return "es";
        }
        case Language::German: {
            return "de";
        }
        case Language::French: {
            return "fr";
        }
        case Language::Italian: {
            return "it";
        }
        case Language::Dutch: {
            return "nl";
        }
        case Language::Japanese: {
            return "ja";
        }
        case Language::Portuguese: {
            return "pt";
        }
        case Language::ChineseSimplified: {
            return "zh-hans";
        }
        case Language::Korean: {
            return "ko";
        }
        case Language::English:
        default: {
            return "en";
        }
    }
}

Language StringToLanguage(std::string str) {
    if(str == "es") {
        return Language::Spanish;
    }
    else if(str == "de") {
        return Language::German;
    }
    else if(str == "fr") {
        return Language::French;
    }
    else if(str == "it") {
        return Language::Italian;
    }
    else if(str == "nl") {
        return Language::Dutch;
    }
    else if(str == "ja") {
        return Language::Japanese;
    }
    else if(str == "pt") {
        return Language::Portuguese;
    }
    else if(str == "zh-hans") {
        return Language::ChineseSimplified;
    }
    else if(str == "ko") {
        return Language::Korean;
    }
    else {
        return Language::English;
    }
}

String Version::AsString() {
    auto as_str = std::to_string(this->major) + "." + std::to_string(this->minor);
    if(this->micro > 0) {
        as_str += "." + std::to_string(this->micro);
    }
    return as_str;
}

Version Version::FromString(String ver_str) {
    auto ver_str_cpy = ver_str;
    Version v = {};
    size_t pos = 0;
    String token;
    u32 c = 0;
    String delimiter = ".";
    while((pos = ver_str_cpy.find(delimiter)) != String::npos) {
        token = ver_str_cpy.substr(0, pos);
        if(c == 0) {
            v.major = std::stoi(token);
        }
        else if(c == 1) {
            v.minor = std::stoi(token);
        }
        else if(c == 2) {
            v.micro = std::stoi(token);
        }
        ver_str_cpy.erase(0, pos + delimiter.length());
        c++;
    }

    if(c == 0) {
        v.major = std::stoi(ver_str_cpy);
    }
    else if(c == 1) {
        v.minor = std::stoi(ver_str_cpy);
    }
    else if(c == 2) {
        v.micro = std::stoi(ver_str_cpy);
    }
    return v;
}

u32 RandomFromRange(u32 min, u32 max) {
    const auto diff = max - min;
    u32 random_val = 0;
    randomGet(&random_val, sizeof(random_val));
    random_val %= (diff + 1);
    return random_val + min;
}

void EnsureDirectories() {
    auto nand_sys_exp = fs::GetNANDSystemExplorer();
    auto sd_exp = fs::GetSdCardExplorer();
    nand_sys_exp->EmptyDirectory("Contents/temp");

    sd_exp->CreateDirectory(consts::Root);
    sd_exp->CreateDirectory(consts::Metadata);
    sd_exp->CreateDirectory(consts::Title);
    sd_exp->CreateDirectory(consts::Dump);
    sd_exp->CreateDirectory(consts::DumpTemp);
    sd_exp->CreateDirectory(consts::DumpUpdate);
    sd_exp->CreateDirectory(consts::DumpTitle);
    sd_exp->CreateDirectory(consts::Reports);
    sd_exp->CreateDirectory(consts::UserData);
}

extern "C" {

    void __appExit();
    void NORETURN __nx_exit(Result rc, LoaderReturnFn ret_addr);

    void NORETURN __libnx_exit(Result rc) {
        void __libc_fini_array(void);
        __libc_fini_array();

        __appExit();

        __nx_exit(rc, envGetExitFuncPtr());
    }

}

void NORETURN Close(Result rc) {
    if(GetLaunchMode() == LaunchMode::Application) {
        libappletRequestHomeMenu();
        __builtin_unreachable();
    }
    else {
        __libnx_exit(rc);
    }
}

Result Initialize() {
    srand(time(nullptr));
    fs::Initialize();
    EnsureDirectories();

    R_TRY(accountInitialize(AccountServiceType_Administrator));
    R_TRY(ncmInitialize());
    R_TRY(avmInitialize());
    R_TRY(nsInitialize());
    R_TRY(es::Initialize());
    R_TRY(psmInitialize());
    R_TRY(setInitialize());
    R_TRY(setsysInitialize());
    R_TRY(usb::detail::Initialize());
    R_TRY(nifmInitialize(NifmServiceType_Admin));
    R_TRY(pdmqryInitialize());
    R_TRY(amssu::Initialize());
    R_TRY(drive::Initialize());
    return 0;
}

void NORETURN Exit(Result rc) {
    if(g_MainApplication) {
        g_MainApplication->Close();
    }

    auto nand_sys_exp = fs::GetNANDSystemExplorer();
    auto nand_safe_exp = fs::GetNANDSafeExplorer();
    auto nand_user_exp = fs::GetNANDUserExplorer();
    auto nand_prodinfof_exp = fs::GetPRODINFOFExplorer();
    auto sd_exp = fs::GetSdCardExplorer();

    if(g_UpdatedNeedsRename) {
        romfsExit();

        const auto cur_nro_file = __system_argv[0];
        sd_exp->DeleteFile(cur_nro_file);
        sd_exp->RenameFile(consts::TempUpdatedNro, cur_nro_file);
    }

    auto work_buf = fs::GetWorkBuffer();
    operator delete[](work_buf, std::align_val_t(0x1000));
    
    delete nand_sys_exp;
    delete nand_safe_exp;
    delete nand_user_exp;
    delete nand_prodinfof_exp;
    delete sd_exp;

    drive::Exit();
    amssu::Exit();
    usb::detail::Exit();
    setsysExit();
    setExit();
    psmExit();
    es::Exit();
    nsExit();
    accountExit();
    avmExit();
    ncmExit();
    nifmExit();
    pdmqryExit();
    Close(rc);
}