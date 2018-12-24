
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <gleaf/sarc.hpp>

namespace gleaf::theme
{
    struct ThemeFileManifest
    {
        int Version;
        std::string Author;
        std::string ThemeName;
        std::string LayoutInfo;
        std::string Target;
        bool UseCommon5X;
    };

    const std::unordered_map<std::string,std::string> ThemeTargetToName
    {
        {"home","Home menu"},
        {"lock","Lock screen"},
        {"user","User page"},
        {"apps","All apps menu"},
        {"set","Settings applet"},
        {"news","News applet"},
    };

    const std::unordered_map<std::string,std::string> ThemeTargetToFileName
    {
        {"home","ResidentMenu.szs"},
        {"lock","Entrance.szs"},
        {"user","MyPage.szs"},
        {"apps","Flaunch.szs"},
        {"set","Set.szs"},
        {"news","Notification.szs"},
    };

    ThemeFileManifest ParseNXThemeFile(sarc::SARC::SarcData &SData);
}