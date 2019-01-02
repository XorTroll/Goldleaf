#include <gleaf/lyt/Patches.hpp>
using namespace std;

namespace gleaf::lyt
{
    bool LayoutPatch::IsCompatible(const sarc::SARC::SarcData &sarc)
    {
        for (int i = 0; i < Files.size(); i++)
        {
            if (!sarc.files.count(Files[i].FileName)) return false;
            // For now this should be enough.
            /*string TargetFileAsString = ASCIIEncoding.ASCII.GetString(szs.Files[Files[i].FileName]);
            for (int j = 0; j < Files[i].Patches.Length; j++)
                if (!TargetFileAsString.Contains(Files[i].Patches[j].PaneName))
                    return false;*/
        }
        return true;
    }

    using json = nlohmann::json;

    LayoutPatch LoadLayout(const string &jsn)
    {
    #define ParseVec3(_n) {_n["X"],_n["Y"],_n["Z"]}
    #define ParseVec2(_n) {_n["X"],_n["Y"]}

        LayoutPatch res;
        auto j = json::parse(jsn);
        if (j.count("PatchName"))
            res.PatchName = j["PatchName"].get<string>();
        if (j.count("AuthorName"))
            res.AuthorName = j["AuthorName"].get<string>();
        if (j.count("Files") && j["Files"].is_array())
        {
            for (auto filePatch : j["Files"])
            {
                if (!filePatch.count("FileName") || !filePatch.count("Patches"))
                    continue;
                LayoutFilePatch p;
                p.FileName = filePatch["FileName"].get<string>();
                for (auto panePatch : filePatch["Patches"])
                {
                    if (!panePatch.count("PaneName"))
                        continue;
                    PanePatch pp;
                    pp.PaneName = panePatch["PaneName"].get<string>();
                    pp.ApplyFlags = 0;
                    if (panePatch.count("Position"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::Position;
                        pp.Position = ParseVec3(panePatch["Position"]);
                    }if (panePatch.count("Rotation"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::Rotation;
                        pp.Rotation = ParseVec3(panePatch["Rotation"]);
                    }if (panePatch.count("Scale"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::Scale;
                        pp.Scale = ParseVec2(panePatch["Scale"]);
                    }if (panePatch.count("Size"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::Size;
                        pp.Size = ParseVec2(panePatch["Size"]);
                    }if (panePatch.count("Visible"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::Visible;
                        pp.Visible = panePatch["Visible"].get<bool>();
                    }

                    if (panePatch.count("ColorTL"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::ColorTL;
                        pp.ColorTL = panePatch["ColorTL"].get<string>();
                    }if (panePatch.count("ColorTR"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::ColorTR;
                        pp.ColorTR = panePatch["ColorTR"].get<string>();
                    }if (panePatch.count("ColorBL"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::ColorBL;
                        pp.ColorBL = panePatch["ColorBL"].get<string>();
                    }if (panePatch.count("ColorBR"))
                    {
                        pp.ApplyFlags |= (u32)PanePatch::Flags::ColorBR;
                        pp.ColorBR = panePatch["ColorBR"].get<string>();
                    }

                    p.Patches.push_back(pp);
                }
                res.Files.push_back(p);
            }
        }
        return res;
    }

    //this is so ugly but c#-like aggregate initialization comes with c++20
    vector < PatchTemplate > DefaultTemplates
    {
        PatchTemplate{
        "home and applets",
        "common.szs",
        "0100000000001000",
        "<= 5.X",
    {},
        {
            "blyt/DHdrSoft.bflyt"
        },
        "blyt/BgNml.bflyt",
        "White1x1_180^r",
        "exelixBG",
    {
        "P_Bg_00"
    },
        {
            "blyt/SystemAppletFader.bflyt"
        },
        "White1x1^r"
    },
    PatchTemplate{
        "home menu",
        "ResidentMenu.szs",
        "0100000000001000",
        "6.X",
    {},
        {
            "anim/RdtBtnShop_LimitB.bflan"
        },
        "blyt/BgNml.bflyt",
        "White1x1A128^s",
        "exelixBG",
    {
        "P_Bg_00"
    },
        {
            "blyt/IconError.bflyt"
        },
        "White1x1A64^t"
    },
    PatchTemplate{
        "lock screen",
        "Entrance.szs",
        "0100000000001000",
        "all firmwares",
    {},
    {},
    "blyt/EntMain.bflyt",
    "White1x1^s",
    "exelixLK",
    {
        "P_BgL",
        "P_BgR"
    },
        {
            "blyt/EntBtnResumeSystemApplet.bflyt"
        },
        "White1x1^r"
    },
    PatchTemplate{
        "user page",
        "MyPage.szs",
        "0100000000001013",
        "all firmwares",
    {
        "blyt/MypUserIconMini.bflyt"
    },
    {},
    "blyt/BaseTop.bflyt",
    "NavBg_03^d",
    "exelixMY",
    {
        "L_AreaNav",
        "L_AreaMain"
    },
        {
            "blyt/BgNav_Root.bflyt"
        },
        "White1x1A0^t"
    },
    PatchTemplate{
        "home menu only",
        "ResidentMenu.szs",
        "0100000000001000",
        "<= 5.X",
    {
        "anim/RdtBtnShop_LimitB.bflan"
    },
    {},
    "blyt/RdtBase.bflyt",
    "White1x1A128^s",
    "exelixResBG",
    {
        "L_BgNml"
    },
        {
            "blyt/IconError.bflyt"
        },
        "White1x1A64^t"
    },
    PatchTemplate{
        "all apps menu",
        "Flaunch.szs",
        "0100000000001000",
        "6.X",
    {
        "blyt/FlcBtnIconGame.bflyt",
        "anim/BaseBg_Loading.bflan"
    }, //anim/BaseBg_Loading.bflan for 6.0
    {},
    "blyt/BgNml.bflyt",
    "NavBg_03^d",
    "exelixFBG",
    {
        "P_Bg_00"
    },
        {
            "blyt/BgNav_Root.bflyt"
        },
        "White1x1A64^t"
    },
    PatchTemplate{
        "settings applet",
        "Set.szs",
        "0100000000001000",
        "6.X",
    {
        "blyt/SetCntDataMngPhoto.bflyt",
        "blyt/SetSideStory.bflyt"
    }, //blyt/SetSideStory.bflyt for 6.0 detection
    {},
    "blyt/BgNml.bflyt",
    "NavBg_03^d",
    "exelixSET",
    {
        "P_Bg_00"
    },
        {
            "blyt/BgNav_Root.bflyt"
        },
        "White1x1A0^t"
    },
    PatchTemplate{
        "news applet",
        "Notification.szs",
        "0100000000001000",
        "6.X",
    {
        "blyt/NtfBase.bflyt",
        "blyt/NtfImage.bflyt"
    }, //blyt/NtfImage.bflyt for 6.0
    {},
    "blyt/BgNml.bflyt",
    "NavBg_03^d",
    "exelixNEW",
    {
        "P_Bg_00"
    },
        {
            "blyt/BgNavNoHeader.bflyt",
            "blyt/BgNav_Root.bflyt"
        },
        "White1x1^r"
    }
    };
}