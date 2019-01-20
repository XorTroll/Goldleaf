
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <string>
#include <vector>
#include <gleaf/sarc.hpp>

namespace gleaf::lyt
{
    struct Vector3 { float X, Y, Z; };
    struct Vector2 { float X, Y; };

    struct PanePatch
    {
        std::string PaneName;
        Vector3 Position, Rotation;
        Vector2 Scale, Size;
        bool Visible;

        std::string ColorTL;
        std::string ColorTR;
        std::string ColorBL;
        std::string ColorBR;

        u32 ApplyFlags; //to disable the properties set to null in the json

        enum class Flags : u32 
        {
            Visible = 1,
            Position = 1 << 1,
            Rotation = 1 << 2,
            Scale = 1 << 3,
            Size = 1 << 4,
            ColorTL = 1 << 5,
            ColorTR = 1 << 6,
            ColorBL = 1 << 7,
            ColorBR = 1 << 8
        };
    };

    struct LayoutFilePatch 
    {
        std::string FileName;
        std::vector<PanePatch> Patches;
    };

    struct LayoutPatch 
    {
        std::string PatchName;
        std::string AuthorName;
        std::vector<LayoutFilePatch> Files;

        bool IsCompatible(const sarc::SARC::SarcData &sarc);
    };

    struct PatchTemplate 
    {
        std::string TemplateName;
        std::string szsName;
        std::string TitleId;
        std::string FirmName;

        std::vector<std::string> FnameIdentifier;
        std::vector<std::string> FnameNotIdentifier;

        std::string MainLayoutName;
        std::string MaintextureName;
        std::string PatchIdentifier;
        std::vector<std::string> targetPanels;
        std::vector<std::string> SecondaryLayouts;
        std::string SecondaryTexReplace;

        bool NoRemovePanel = false;
    };

    extern std::vector<PatchTemplate> DefaultTemplates;
    LayoutPatch LoadLayout(const std::string &json);
}