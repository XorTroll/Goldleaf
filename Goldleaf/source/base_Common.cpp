
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

namespace {

    constexpr Language DefaultLanguage = Language::English;

    std::map<Language, std::vector<std::string>> g_LanguageNamesMap = {
        { Language::English, { "AmericanEnglish", "BritishEnglish" } },
        { Language::Spanish, { "Spanish", "LatinAmericanSpanish" } },
        { Language::German, { "German" } },
        { Language::French, { "French", "CanadianFrench" } },
        { Language::Italian, { "Italian" } },
        { Language::Dutch, { "Dutch" } },
        { Language::Japanese, { "Japanese" } },
        { Language::Portuguese, { "Portuguese", "BrazilianPortuguese" } },
        { Language::ChineseSimplified, { "SimplifiedChinese" } },
        { Language::ChineseTraditional, { "TraditionalChinese" } },
        { Language::Korean, { "Korean" } },
    };

    std::map<Language, std::vector<std::string>> g_LanguageCodesMap = {
        { Language::English, { "en-US", "en-GB" } },
        { Language::Spanish, { "es", "es-419" } },
        { Language::German, { "de" } },
        { Language::French, { "fr", "fr-CA" } },
        { Language::Italian, { "it" } },
        { Language::Dutch, { "nl" } },
        { Language::Japanese, { "ja" } },
        { Language::Portuguese, { "pt", "pt-BR" } },
        { Language::ChineseSimplified, { "zh-Hans" } },
        { Language::ChineseTraditional, { "zh-Hant" } },
        { Language::Korean, { "ko" } },
    };

    std::map<Language, std::vector<SetLanguage>> g_SystemLanguagesMap = {
        { Language::English, { SetLanguage_ENUS, SetLanguage_ENGB } },
        { Language::Spanish, { SetLanguage_ES, SetLanguage_ES419 } },
        { Language::German, { SetLanguage_DE } },
        { Language::French, { SetLanguage_FR, SetLanguage_FRCA } },
        { Language::Italian, { SetLanguage_IT } },
        { Language::Dutch, { SetLanguage_NL } },
        { Language::Japanese, { SetLanguage_JA } },
        { Language::Portuguese, { SetLanguage_PT, SetLanguage_PTBR } },
        { Language::ChineseSimplified, { SetLanguage_ZHHANS } },
        { Language::ChineseTraditional, { SetLanguage_ZHHANT } },
        { Language::Korean, { SetLanguage_KO } },
    };

}

std::string LowerCaseString(const std::string &str) {
    auto copy = str;
    std::transform(copy.begin(), copy.end(), copy.begin(), tolower);
    return copy;
}

Language GetLanguageBySystemLanguage(const SetLanguage sys_lang) {
    for(const auto &[lang, sys_langs] : g_SystemLanguagesMap) {
        for(const auto &s_sys_lang: sys_langs) {
            if(sys_lang == s_sys_lang) {
                return lang;
            }
        }
    }

    return DefaultLanguage;
}

std::string GetLanguageCode(const Language lang) {
    const auto lang_code_it = g_LanguageCodesMap.find(lang);
    if(lang_code_it != g_LanguageCodesMap.end()) {
        return lang_code_it->second.front();
    }
    else {
        return g_LanguageCodesMap.at(DefaultLanguage).front();
    }
}

Language GetLanguageByCode(const std::string &lang_code) {
    for(const auto &[lang, codes] : g_LanguageCodesMap) {
        for(const auto &code: codes) {
            if(lang_code == code) {
                return lang;
            }
        }
    }

    return DefaultLanguage;
}

std::string GetLanguageName(const Language lang) {
    const auto lang_name_it = g_LanguageNamesMap.find(lang);
    if(lang_name_it != g_LanguageNamesMap.end()) {
        return lang_name_it->second.front();
    }
    else {
        return g_LanguageNamesMap.at(DefaultLanguage).front();
    }
}

Language GetLanguageByName(const std::string &lang_name) {
    for(const auto &[lang, names] : g_LanguageNamesMap) {
        for(const auto &name: names) {
            if(name == lang_name) {
                return lang;
            }
        }
    }

    return DefaultLanguage;
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
    sd_exp->CreateDirectory(GLEAF_PATH_EXPORT_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_EXPORT_TEMP_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_EXPORT_UPDATE_DIR);
    sd_exp->CreateDirectory(GLEAF_PATH_EXPORT_TITLE_DIR);
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