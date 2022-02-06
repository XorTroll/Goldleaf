
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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
#include <usb/usb_Base.hpp>
#include <es/es_Service.hpp>
#include <amssu/amssu_Service.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;

extern char **__system_argv;
extern bool g_UpdatedNeedsRename;

std::string LowerCaseString(const std::string &str) {
    auto copy = str;
    std::transform(copy.begin(), copy.end(), copy.begin(), tolower);
    return copy;
}

std::string LanguageToString(const Language lang) {
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

Language StringToLanguage(const std::string &str) {
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

std::string Version::AsString() {
    auto as_str = std::to_string(this->major) + "." + std::to_string(this->minor);
    if(this->micro > 0) {
        as_str += "." + std::to_string(this->micro);
    }
    return as_str;
}

Version Version::FromString(const std::string &ver_str) {
    auto ver_str_cpy = ver_str;
    Version v = {};
    size_t pos = 0;
    std::string token;
    u32 c = 0;
    std::string delimiter = ".";
    while((pos = ver_str_cpy.find(delimiter)) != std::string::npos) {
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

u32 RandomFromRange(const u32 min, const u32 max) {
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

    sd_exp->CreateDirectory(GLEAF_PATH_ROOT_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_METADATA_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_TITLE_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_DUMP_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_DUMP_TEMP_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_DUMP_UPDATE_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_DUMP_TITLE_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_REPORTS_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_USER_DATA_DIR);
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

Result Initialize() {
    srand(time(nullptr));
    fs::Initialize();
    EnsureDirectories();

    GLEAF_RC_TRY(accountInitialize(AccountServiceType_Administrator));
    GLEAF_RC_TRY(ncmInitialize());
    GLEAF_RC_TRY(avmInitialize());
    GLEAF_RC_TRY(nsInitialize());
    GLEAF_RC_TRY(es::Initialize());
    GLEAF_RC_TRY(psmInitialize());
    GLEAF_RC_TRY(setInitialize());
    GLEAF_RC_TRY(setsysInitialize());
    GLEAF_RC_TRY(usb::Initialize());
    GLEAF_RC_TRY(nifmInitialize(NifmServiceType_Admin));
    GLEAF_RC_TRY(pdmqryInitialize());
    GLEAF_RC_TRY(amssu::Initialize());
    GLEAF_RC_TRY(drive::Initialize());
    return 0;
}

void NORETURN Exit(const Result rc) {
    if(g_MainApplication) {
        g_MainApplication->Close();
    }

    if(g_UpdatedNeedsRename) {
        romfsExit();

        const auto cur_nro_file = __system_argv[0];
        auto sd_exp = fs::GetSdCardExplorer();
        sd_exp->DeleteFile(cur_nro_file);
        sd_exp->RenameFile(GLEAF_PATH_TEMP_UPDATE_NRO, cur_nro_file);
    }

    fs::Finalize();
    drive::Exit();
    amssu::Exit();
    usb::Exit();
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

    __libnx_exit(rc);
}

void LogImpl(const char *log_buf, const size_t log_buf_len) {
    auto sd_exp = fs::GetSdCardExplorer();
    sd_exp->WriteFile(GLEAF_PATH_LOG_FILE, log_buf, log_buf_len);
}