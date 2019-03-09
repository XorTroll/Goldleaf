#include <gleaf/set/Settings.hpp>
#include <gleaf/ui.hpp>
#include <gleaf/ini.hpp>
#include <gleaf/fs.hpp>

namespace gleaf::set
{
    std::string Settings::PathForResource(std::string Path)
    {
        std::string outres = "romfs:" + Path;
        if(!this->RomFsReplacePath.empty())
        {
            std::string tmpres = this->RomFsReplacePath + "/" + Path;
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
        gset.KeysPath = "sdmc:/switch/prod.keys";
        gset.AllowRemoveSystemTitles = false;
        gset.WarnRemoveUsedTickets = true;
        gset.RomFsReplacePath = "";
        ColorSetId csid = ColorSetId_Light;
        setsysGetColorSetId(&csid);
        if(csid == ColorSetId_Dark) gset.CustomScheme = ui::DefaultDark;
        else gset.CustomScheme = ui::DefaultLight;
        std::string pini = "sdmc:/goldleaf/goldleaf.ini";
        if(fs::IsFile(pini))
        {
            INIReader inir(pini);
            bool usecl = inir.GetBoolean("General", "useCustomLanguage", false);
            if(usecl)
            {
                std::string lang = inir.Get("General", "customLanguage", "en");
                if(lang == "en") gset.CustomLanguage = Language::English;
                else if(lang == "es") gset.CustomLanguage = Language::Spanish;
                else if(lang == "de") gset.CustomLanguage = Language::German;
                else if(lang == "fr") gset.CustomLanguage = Language::French;
                else if(lang == "it") gset.CustomLanguage = Language::Italian;
            }
            gset.KeysPath = "sdmc:/" + inir.Get("General", "keysPath", "switch/prod.keys");
            gset.AllowRemoveSystemTitles = inir.GetBoolean("Content", "allowRemoveSystemTitles", false);
            gset.WarnRemoveUsedTickets = inir.GetBoolean("Content", "warnRemoveUsedTickets", true);
            std::string prom = inir.Get("UI", "romFsReplacePath", "");
            if(!prom.empty()) gset.RomFsReplacePath = "sdmc:/" + prom;
            else gset.RomFsReplacePath = prom;
            bool usecc = inir.GetBoolean("UI", "useCustomColors", false);
            if(usecc)
            {
                std::string rawcbg = inir.Get("UI", "colorBackground", "");
                std::string rawcbs = inir.Get("UI", "colorBase", "");
                std::string rawcbf = inir.Get("UI", "colorBaseFocus", "");
                std::string rawctx = inir.Get("UI", "colorText", "");
                size_t ipos = 0;
                u8 step = 0;
                if(!rawcbg.empty()) while((ipos = rawcbg.find(",")) != std::string::npos)
                {
                    std::string clr = rawcbg.substr(0, ipos);
                    if(step == 0) gset.CustomScheme.Background.R = (u8)atoi(clr.c_str());
                    else if(step == 1) gset.CustomScheme.Background.G = (u8)atoi(clr.c_str());
                    else if(step == 2) gset.CustomScheme.Background.B = (u8)atoi(clr.c_str());
                    else if(step == 3) gset.CustomScheme.Background.A = (u8)atoi(clr.c_str());
                    step++;
                    rawcbg.erase(0, ipos + 1);
                }
                step = 0;
                if(!rawcbs.empty()) while((ipos = rawcbs.find(",")) != std::string::npos)
                {
                    std::string clr = rawcbs.substr(0, ipos);
                    if(step == 0) gset.CustomScheme.Base.R = (u8)atoi(clr.c_str());
                    else if(step == 1) gset.CustomScheme.Base.G = (u8)atoi(clr.c_str());
                    else if(step == 2) gset.CustomScheme.Base.B = (u8)atoi(clr.c_str());
                    else if(step == 3) gset.CustomScheme.Base.A = (u8)atoi(clr.c_str());
                    step++;
                    rawcbs.erase(0, ipos + 1);
                }
                step = 0;
                if(!rawcbf.empty()) while((ipos = rawcbf.find(",")) != std::string::npos)
                {
                    std::string clr = rawcbf.substr(0, ipos);
                    if(step == 0) gset.CustomScheme.BaseFocus.R = (u8)atoi(clr.c_str());
                    else if(step == 1) gset.CustomScheme.BaseFocus.G = (u8)atoi(clr.c_str());
                    else if(step == 2) gset.CustomScheme.BaseFocus.B = (u8)atoi(clr.c_str());
                    else if(step == 3) gset.CustomScheme.BaseFocus.A = (u8)atoi(clr.c_str());
                    step++;
                    rawcbf.erase(0, ipos + 1);
                }
                step = 0;
                if(!rawctx.empty()) while((ipos = rawctx.find(",")) != std::string::npos)
                {
                    std::string clr = rawctx.substr(0, ipos);
                    if(step == 0) gset.CustomScheme.Text.R = (u8)atoi(clr.c_str());
                    else if(step == 1) gset.CustomScheme.Text.G = (u8)atoi(clr.c_str());
                    else if(step == 2) gset.CustomScheme.Text.B = (u8)atoi(clr.c_str());
                    else if(step == 3) gset.CustomScheme.Text.A = (u8)atoi(clr.c_str());
                    step++;
                    rawctx.erase(0, ipos + 1);
                }
            }
        }
        return gset;
    }

    bool Exists()
    {
        return fs::IsFile("sdmc:/goldleaf/goldleaf.ini");
    }
}
