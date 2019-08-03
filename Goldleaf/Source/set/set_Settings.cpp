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

    Settings ProcessSettings()
    {
        Settings gset;
        u64 lcode = 0;
        s32 lang = 1;
        setGetSystemLanguage(&lcode);
        setMakeLanguage(lcode, &lang);
        switch(lang)
        {
            // case 0:
            case 1:
            case 12:
                gset.CustomLanguage = Language::English;
                break;
            case 2:
            case 13:
                gset.CustomLanguage = Language::French;
                break;
            case 3:
                gset.CustomLanguage = Language::German;
                break;
            case 4:
                gset.CustomLanguage = Language::Italian;
                break;
            case 5:
            case 14:
                gset.CustomLanguage = Language::Spanish;
                break;
            /*
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 15:
            case 16:    
            */
            default:
                gset.CustomLanguage = Language::English;
                break;
        }
        gset.MenuItemSize = 80;
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
            }
            if(settings.count("installs"))
            {
                gset.IgnoreRequiredFirmwareVersion = settings["installs"].value("ignoreRequiredFwVersion", false);
                // More for 0.7!
            }
            /* 0.7
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
