
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

#include <ui/ui_Utils.hpp>
#include <ui/ui_MainApplication.hpp>

set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;
    pu::String clipboard;

    void SetClipboard(pu::String Path)
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

    void ShowPowerTasksDialog(pu::String Title, pu::String Message)
    {
        int sopt = mainapp->CreateShowDialog(Title, Message, { set::GetDictionaryEntry(233), set::GetDictionaryEntry(232), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        else switch(sopt)
        {
            case 0:
                bpcShutdownSystem();
                break;
            case 1:
                bpcRebootSystem();
                break;
        }
    }

    pu::String AskForText(pu::String Guide, pu::String Initial, int MaxSize)
    {
        pu::String out = "";
        char tmpout[FS_MAX_PATH] = { 0 };
        SwkbdConfig kbd;
        Result rc = swkbdCreate(&kbd, 0);
        if(rc == 0)
        {
            swkbdConfigMakePresetDefault(&kbd);
            if(MaxSize > 0) swkbdConfigSetStringLenMax(&kbd, (u32)MaxSize);
            if(Guide != "") swkbdConfigSetGuideText(&kbd, Guide.AsUTF8().c_str());
            if(Initial != "") swkbdConfigSetInitialText(&kbd, Initial.AsUTF8().c_str());
            rc = swkbdShow(&kbd, tmpout, sizeof(tmpout));
            if(rc == 0) out = pu::String(tmpout);
        }
        swkbdClose(&kbd);
        return out;
    }

    void HandleResult(Result OSError, pu::String Context)
    {
        if(OSError != 0)
        {
            err::Error err = err::DetermineError(OSError);
            char displayerr[0x10] = {0};
            sprintf(displayerr, "%04d-%04d", 2000 + R_MODULE(err.OSError), R_DESCRIPTION(err.OSError));
            pu::String emod = err.Module + " (" + std::to_string(R_MODULE(err.OSError)) + ")";
            pu::String edesc = err.Description + " (" + std::to_string(R_DESCRIPTION(err.OSError)) + ")";
            mainapp->CreateShowDialog(set::GetDictionaryEntry(266), Context + "\n\n" + set::GetDictionaryEntry(266) + ": " + std::string(displayerr) + " (" + hos::FormatHex(err.OSError) + ")\n" + set::GetDictionaryEntry(264) + ": " + emod + "\n" + set::GetDictionaryEntry(265) + ": " + edesc + "", { set::GetDictionaryEntry(234) }, false);
        }
    }
}