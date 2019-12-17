
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

#include <set/set_Settings.hpp>
#include <ui/ui_MainApplication.hpp>
#include <fs/fs_Explorer.hpp>

namespace set
{
    std::string Settings::PathForResource(std::string Path)
    {
        std::string outres = "romfs:" + Path;
        if(!this->ExternalRomFs.empty())
        {
            std::string tmpres = this->ExternalRomFs + "/" + Path;
            if(fs::IsFile(tmpres)) outres = tmpres;
        }
        return outres;
    }

    void Settings::ApplyScrollBarColor(pu::ui::elm::Menu::Ref &Menu)
    {
        if(this->HasScrollBar) Menu->SetScrollbarColor(this->ScrollBarColor);
    }

    void Settings::ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &PBar)
    {
        if(this->HasProgressBar) PBar->SetProgressColor(this->ProgressBarColor);
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
                gset.CustomLanguage = Language::English;
                break;
            case SetLanguage_FR:
            case SetLanguage_FRCA:
                gset.CustomLanguage = Language::French;
                break;
            case SetLanguage_DE:
                gset.CustomLanguage = Language::German;
                break;
            case SetLanguage_IT:
                gset.CustomLanguage = Language::Italian;
                break;
            case SetLanguage_ES:
            case SetLanguage_ES419:
                gset.CustomLanguage = Language::Spanish;
                break;
            case SetLanguage_NL:
                gset.CustomLanguage = Language::Dutch;
                break;
            default:
                gset.CustomLanguage = Language::English;
                break;
        }
        gset.MenuItemSize = 80;
        gset.HasScrollBar = false;
        gset.HasProgressBar = false;
        gset.IgnoreRequiredFirmwareVersion = true;
        ColorSetId csid = ColorSetId_Light;
        setsysGetColorSetId(&csid);
        if(csid == ColorSetId_Dark) gset.CustomScheme = ui::DefaultDark;
        else gset.CustomScheme = ui::DefaultLight;
        std::ifstream ifs(SettingsFile);
        if(ifs.good())
        {
            JSON settings = JSON::parse(ifs);
            if(settings.count("general"))
            {
                std::string lang = settings["general"].value("customLanguage", "");
                if(!lang.empty())
                {
                    if(lang == "en") gset.CustomLanguage = Language::English;
                    else if(lang == "es") gset.CustomLanguage = Language::Spanish;
                    else if(lang == "de") gset.CustomLanguage = Language::German;
                    else if(lang == "fr") gset.CustomLanguage = Language::French;
                    else if(lang == "it") gset.CustomLanguage = Language::Italian;
                    else if(lang == "nl") gset.CustomLanguage = Language::Dutch;
                }
                std::string keys = settings["general"].value("keysPath", "");
                if(!keys.empty())
                {
                    gset.KeysPath = "sdmc:";
                    if(keys[0] != '/') gset.KeysPath += "/";
                    gset.KeysPath += keys;
                }
                std::string extrom = settings["general"].value("externalRomFs", "");
                if(!extrom.empty())
                {
                    gset.ExternalRomFs = "sdmc:";
                    if(extrom[0] != '/') gset.ExternalRomFs += "/";
                    gset.ExternalRomFs += extrom;
                }
            }
            if(settings.count("ui"))
            {
                std::string clr = settings["ui"].value("background", "");
                if(!clr.empty()) gset.CustomScheme.Background = pu::ui::Color::FromHex(clr);
                clr = settings["ui"].value("base", "");
                if(!clr.empty()) gset.CustomScheme.Base = pu::ui::Color::FromHex(clr);
                clr = settings["ui"].value("baseFocus", "");
                if(!clr.empty()) gset.CustomScheme.BaseFocus = pu::ui::Color::FromHex(clr);
                clr = settings["ui"].value("text", "");
                if(!clr.empty()) gset.CustomScheme.Text = pu::ui::Color::FromHex(clr);
                clr = settings["ui"].value("scrollBar", "");
                if(!clr.empty())
                {
                    gset.HasScrollBar = true;
                    gset.ScrollBarColor = pu::ui::Color::FromHex(clr);
                }
                clr = settings["ui"].value("progressBar", "");
                if(!clr.empty())
                {
                    gset.HasProgressBar = true;
                    gset.ProgressBarColor = pu::ui::Color::FromHex(clr);
                }
            }
            if(settings.count("installs"))
            {
                gset.IgnoreRequiredFirmwareVersion = settings["installs"].value("ignoreRequiredFwVersion", true);
                // More for 0.8!
            }
            /* 0.8
            if(settings.count("web"))
            {
                if(settings["web"].count("bookmarks"))
                {
                    for(u32 i = 0; i < settings["web"]["bookmarks"].size(); i++)
                    {
                        WebBookmark bmk;
                        bmk.Name = settings["web"]["bookmarks"][i].value("name", "");
                        bmk.URL = settings["web"]["bookmarks"][i].value("url", "");
                        if(!bmk.URL.empty() && !bmk.Name.empty())
                        {
                            gset.Bookmarks.push_back(bmk);
                        }
                    }
                }
            }
            */
            ifs.close();
        }
        return gset;
    }

    bool Exists()
    {
        return fs::IsFile(SettingsFile);
    }
}
