 
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

#include <ui/ui_Utils.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    String clipboard;

    void SetClipboard(String Path)
    {
        clipboard = Path;
    }
    
    void ClearClipboard()
    {
        clipboard = "";
    }

    bool ClipboardEmpty()
    {
        return clipboard.empty();
    }
    
    bool ClipboardNotEmpty()
    {
        return !clipboard.empty();
    }

    void ShowPowerTasksDialog(String Title, String Message)
    {
        int sopt = global_app->CreateShowDialog(Title, Message, { cfg::strings::Main.GetString(233), cfg::strings::Main.GetString(232), cfg::strings::Main.GetString(18) }, true);
        if(sopt < 0) return;
        spsmInitialize();
        spsmShutdown(sopt == 1);
        spsmExit();
    }

    String AskForText(String Guide, String Initial, int MaxSize)
    {
        String out = "";
        char tmpout[FS_MAX_PATH] = { 0 };
        SwkbdConfig kbd;
        Result rc = swkbdCreate(&kbd, 0);
        if(R_SUCCEEDED(rc))
        {
            swkbdConfigMakePresetDefault(&kbd);
            if(MaxSize > 0) swkbdConfigSetStringLenMax(&kbd, (u32)MaxSize);
            if(Guide != "") swkbdConfigSetGuideText(&kbd, Guide.AsUTF8().c_str());
            if(Initial != "") swkbdConfigSetInitialText(&kbd, Initial.AsUTF8().c_str());
            rc = swkbdShow(&kbd, tmpout, sizeof(tmpout));
            if(R_SUCCEEDED(rc)) out = String(tmpout);
        }
        swkbdClose(&kbd);
        return out;
    }

    void HandleResult(Result rc, String info)
    {
        if(R_FAILED(rc))
        {
            auto serr = hos::FormatResult(rc);
            auto sres = err::GetResultDescription(rc);
            auto modname = err::GetModuleName(R_MODULE(rc));
            auto infomod = modname + " (" + std::to_string(R_MODULE(rc)) + ")";
            auto infodesc = sres + " (" + std::to_string(R_DESCRIPTION(rc)) + ")";
            global_app->CreateShowDialog(cfg::strings::Main.GetString(266), info + "\n\n" + cfg::strings::Main.GetString(266) + ": " + serr + " (" + hos::FormatHex(rc) + ")\n" + cfg::strings::Main.GetString(264) + ": " + infomod + "\n" + cfg::strings::Main.GetString(265) + ": " + infodesc + "", { cfg::strings::Main.GetString(234) }, false);
        }
    }

    inline u8 VariateImpl(u8 input, u8 v)
    {
        u32 tmp = (u32)input + v;
        if(tmp > 255) return 255;
        return (u8)tmp;
    }

    inline pu::ui::Color GenerateVariation(pu::ui::Color clr, u8 min_v, u8 max_v)
    {
        auto v = (u8)RandomFromRange(min_v, max_v);
        return { VariateImpl(clr.R, v), VariateImpl(clr.G, v), VariateImpl(clr.B, v), clr.A };
    }

    inline bool IsColorDark(pu::ui::Color clr)
    {
        u8 low_cmp_no = 0;
        if(clr.R < 128) low_cmp_no++;
        if(clr.G < 128) low_cmp_no++;
        if(clr.B < 128) low_cmp_no++;
        return low_cmp_no >= 2;
    }

    ColorScheme GenerateRandomScheme()
    {
        ColorScheme scheme = {};
        u8 r = (u8)RandomFromRange(0, 0xff);
        u8 g = (u8)RandomFromRange(0, 0xff);
        u8 b = (u8)RandomFromRange(0, 0xff);
        pu::ui::Color clr = { r, g, b, 0xff };
        scheme.Base = clr;
        scheme.Background = GenerateVariation(clr, 30, 50);
        scheme.BaseFocus = GenerateVariation(clr, 20, 30);
        auto av = (r + g + b) / 3;
        if(av < 128) scheme.Text = { 225, 225, 225, 255 };
        else scheme.Text = { 15, 15, 15, 255 };
        return scheme;
    }
}