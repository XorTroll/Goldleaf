
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

#include <cfg/cfg_Settings.hpp>
#include <ui/ui_MainApplication.hpp>
#include <fs/fs_FileSystem.hpp>
#include <iomanip>

namespace cfg
{
    inline std::string ColorToHex(pu::ui::Color clr)
    {
        char str[0x20] = {0};
        sprintf(str, "#%02X%02X%02X%02X", clr.R, clr.G, clr.B, clr.A);
        return str;
    }

    void Settings::Save()
    {
        auto json = JSON::object();
        if(this->has_custom_lang) json["general"]["customLanguage"] = LanguageToString(this->custom_lang);
        if(this->has_external_romfs) json["general"]["externalRomFs"] = this->external_romfs;
        if(this->has_menu_item_size) json["ui"]["menuItemSize"] = this->menu_item_size;
        if(this->has_custom_scheme)
        {
            json["ui"]["base"] = ColorToHex(this->custom_scheme.Base);
            json["ui"]["baseFocus"] = ColorToHex(this->custom_scheme.BaseFocus);
            json["ui"]["text"] = ColorToHex(this->custom_scheme.Text);
        }
        if(this->has_scrollbar_color) json["ui"]["scrollBar"] = ColorToHex(this->scrollbar_color);
        if(this->has_progressbar_color) json["ui"]["progressBar"] = ColorToHex(this->progressbar_color);
        json["installs"]["ignoreRequiredFwVersion"] = this->ignore_required_fw_ver;
        for(u32 i = 0; i < this->bookmarks.size(); i++)
        {
            auto bmk = this->bookmarks[i];
            json["web"]["bookmarks"][i]["name"] = bmk.name;
            json["web"]["bookmarks"][i]["url"] = bmk.url;
        }
        auto sd_exp = fs::GetSdCardExplorer();
        sd_exp->DeleteFile("sdmc:/" + consts::Settings);
        std::ofstream ofs("sdmc:/" + consts::Settings);
        ofs << std::setw(4) << json;
        ofs.close();
    }

    std::string Settings::PathForResource(std::string Path)
    {
        auto outres = "romfs:" + Path;
        if(this->has_external_romfs)
        {
            auto tmpres = this->external_romfs + "/" + Path;
            auto sd_exp = fs::GetSdCardExplorer();
            if(sd_exp->IsFile(tmpres)) outres = tmpres;
        }
        return outres;
    }

    void Settings::ApplyScrollBarColor(pu::ui::elm::Menu::Ref &Menu)
    {
        if(this->has_scrollbar_color) Menu->SetScrollbarColor(this->scrollbar_color);
    }

    void Settings::ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &PBar)
    {
        if(this->has_progressbar_color) PBar->SetProgressColor(this->progressbar_color);
    }

    Settings ProcessSettings()
    {
        Settings gset;
        u64 lcode = 0;
        SetLanguage lang = SetLanguage_ENUS;
        setGetSystemLanguage(&lcode);
        setMakeLanguage(lcode, &lang);
        switch(lang)
        {
            case SetLanguage_ENUS:
            case SetLanguage_ENGB:
                gset.custom_lang = Language::English;
                break;
            case SetLanguage_FR:
            case SetLanguage_FRCA:
                gset.custom_lang = Language::French;
                break;
            case SetLanguage_DE:
                gset.custom_lang = Language::German;
                break;
            case SetLanguage_IT:
                gset.custom_lang = Language::Italian;
                break;
            case SetLanguage_ES:
            case SetLanguage_ES419:
                gset.custom_lang = Language::Spanish;
                break;
            case SetLanguage_NL:
                gset.custom_lang = Language::Dutch;
                break;
            default:
                gset.custom_lang = Language::English;
                break;
        }
        gset.has_custom_lang = false;
        gset.has_external_romfs = false;
        gset.has_scrollbar_color = false;
        gset.has_progressbar_color = false;
        gset.has_custom_scheme = false;
        gset.has_menu_item_size = false;

        gset.menu_item_size = 80;
        gset.ignore_required_fw_ver = true;

        ColorSetId csid = ColorSetId_Light;
        setsysGetColorSetId(&csid);
        if(csid == ColorSetId_Dark) gset.custom_scheme = ui::DefaultDark;
        else gset.custom_scheme = ui::DefaultLight;

        std::ifstream ifs("sdmc:/" + consts::Settings);
        if(ifs.good())
        {
            JSON settings = JSON::parse(ifs);
            if(settings.count("general"))
            {
                std::string lang = settings["general"].value("customLanguage", "");
                if(!lang.empty())
                {
                    auto clang = StringToLanguage(lang);
                    gset.has_custom_lang = true;
                    gset.custom_lang = clang;
                }
                std::string extrom = settings["general"].value("externalRomFs", "");
                if(!extrom.empty())
                {
                    gset.has_external_romfs = true;
                    if(extrom.substr(0, 6) == "sdmc:/") gset.external_romfs = extrom;
                    else
                    {
                        gset.external_romfs = "sdmc:";
                        if(extrom[0] != '/') gset.external_romfs += "/";
                        gset.external_romfs += extrom;
                    }
                }
            }
            if(settings.count("ui"))
            {
                auto itemsize = settings["ui"].value("menuItemSize", 0);
                if(itemsize > 0)
                {
                    gset.has_menu_item_size = true;
                    gset.menu_item_size = itemsize;
                }
                std::string clr = settings["ui"].value("background", "");
                if(!clr.empty())
                {
                    gset.has_custom_scheme = true;
                    gset.custom_scheme.Background = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("base", "");
                if(!clr.empty())
                {
                    gset.has_custom_scheme = true;
                    gset.custom_scheme.Base = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("baseFocus", "");
                if(!clr.empty())
                {
                    gset.has_custom_scheme = true;
                    gset.custom_scheme.BaseFocus = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("text", "");
                if(!clr.empty())
                {
                    gset.has_custom_scheme = true;
                    gset.custom_scheme.Text = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("scrollBar", "");
                if(!clr.empty())
                {
                    gset.has_scrollbar_color = true;
                    gset.scrollbar_color = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("progressBar", "");
                if(!clr.empty())
                {
                    gset.has_progressbar_color = true;
                    gset.progressbar_color = pu::ui::Color::FromHex(clr);
                }
            }
            if(settings.count("installs"))
            {
                gset.ignore_required_fw_ver = settings["installs"].value("ignoreRequiredFwVersion", true);
            }
            if(settings.count("web"))
            {
                if(settings["web"].count("bookmarks"))
                {
                    for(u32 i = 0; i < settings["web"]["bookmarks"].size(); i++)
                    {
                        WebBookmark bmk = {};
                        bmk.name = settings["web"]["bookmarks"][i].value("name", "");
                        bmk.url = settings["web"]["bookmarks"][i].value("url", "");
                        if(!bmk.url.empty() && !bmk.name.empty())
                        {
                            gset.bookmarks.push_back(bmk);
                        }
                    }
                }
            }
            ifs.close();
        }
        return gset;
    }

    bool Exists()
    {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->IsFile(consts::Settings);
    }
}
