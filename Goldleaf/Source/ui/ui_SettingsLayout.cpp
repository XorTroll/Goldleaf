
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

#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    SettingsLayout::SettingsLayout() : pu::ui::Layout()
    {
        this->optsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->optsMenu);
        auto itm = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(352));
        itm->SetColor(gsets.CustomScheme.Text);
        itm->AddOnClick(std::bind(&SettingsLayout::optsFirmware_Click, this));
        this->optsMenu->AddItem(itm);
        auto itm2 = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(353));
        itm2->SetColor(gsets.CustomScheme.Text);
        itm2->AddOnClick(std::bind(&SettingsLayout::optsMemory_Click, this));
        this->optsMenu->AddItem(itm2);
        auto itm3 = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(354));
        itm3->SetColor(gsets.CustomScheme.Text);
        itm3->AddOnClick(std::bind(&SettingsLayout::optsConfig_Click, this));
        this->optsMenu->AddItem(itm3);
        this->progressInfo = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->progressInfo->SetVisible(false);
        gsets.ApplyProgressBarColor(this->progressInfo);
        this->Add(this->optsMenu);
        this->Add(this->progressInfo);
    }

    void SettingsLayout::optsConfig_Click()
    {
        pu::String msg = set::GetDictionaryEntry(354) + ":\n";
        msg += pu::String("\n" + set::GetDictionaryEntry(355) + ": ") + (gsets.IgnoreRequiredFirmwareVersion ? set::GetDictionaryEntry(111) : set::GetDictionaryEntry(112));
        if(!gsets.ExternalRomFs.empty()) msg += "\n" + set::GetDictionaryEntry(356) + ": 'SdCard:/" + gsets.ExternalRomFs + "'";
        mainapp->CreateShowDialog(set::GetDictionaryEntry(357), msg, { set::GetDictionaryEntry(234) }, true);
    }

    void SettingsLayout::ExportUpdateToDirectory(pu::String Input, SetSysFirmwareVersion Fw)
    {
        auto sd = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->optsMenu->SetVisible(false);
        this->progressInfo->SetVisible(true);
        mainapp->LoadMenuHead(set::GetDictionaryEntry(359) + " " + Fw.display_version + "...");
        auto outdir = sd->FullPathFor(GoldleafDir + "/dump/update/" + Fw.display_version);
        sd->DeleteDirectory(outdir);
        exp->CopyDirectoryProgress(Input, outdir, [&](double Done, double Total)
        {
            this->progressInfo->SetMaxValue(Total);
            this->progressInfo->SetProgress(Done);
            mainapp->CallForRender();
        });
        mainapp->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
        this->optsMenu->SetVisible(true);
        this->progressInfo->SetVisible(false);
        mainapp->ShowNotification(set::GetDictionaryEntry(358) + " '" + outdir + "'.");
    }

    void SettingsLayout::ExportUpdateToNSP(pu::String Input, SetSysFirmwareVersion Fw)
    {
        auto sd = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->optsMenu->SetVisible(false);
        this->progressInfo->SetVisible(true);
        mainapp->LoadMenuHead(set::GetDictionaryEntry(359) + " " + Fw.display_version + "...");
        auto outnsp = sd->FullPathFor(GoldleafDir + "/dump/update/" + Fw.display_version + ".nsp");
        sd->DeleteFile(outnsp);
        nsp::GenerateFrom(exp->FullPathFor(Input), outnsp, [&](u64 Done, u64 Total)
        {
            this->progressInfo->SetMaxValue((double)Total);
            this->progressInfo->SetProgress((double)Done);
            mainapp->CallForRender();
        });
        mainapp->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
        this->optsMenu->SetVisible(true);
        this->progressInfo->SetVisible(false);
        mainapp->ShowNotification(set::GetDictionaryEntry(358) + " '" + outnsp + "'.");
    }

    void SettingsLayout::HandleUpdate(pu::String Base, SetSysFirmwareVersion Fw)
    {
        auto sopt2 = mainapp->CreateShowDialog(set::GetDictionaryEntry(360), set::GetDictionaryEntry(361), { set::GetDictionaryEntry(377), set::GetDictionaryEntry(53), set::GetDictionaryEntry(18)}, true);
        if(sopt2 == 0) ExportUpdateToDirectory(Base, Fw);
        else if(sopt2 == 1) ExportUpdateToNSP(Base, Fw);
    }

    void SettingsLayout::optsFirmware_Click()
    {
        SetSysFirmwareVersion fwver = {};
        setsysGetFirmwareVersion(&fwver);
        pu::String msg = set::GetDictionaryEntry(362) + ":\n";
        msg += pu::String("\n" + set::GetDictionaryEntry(363) + ": ") + fwver.display_version + " (" + fwver.display_title + ")";
        msg += pu::String("\n" + set::GetDictionaryEntry(364) + ": '") + fwver.version_hash + "'";
        msg += pu::String("\n" + set::GetDictionaryEntry(95) + " ") + std::to_string(hos::ComputeSystemKeyGeneration());
        msg += "\n\n" + set::GetDictionaryEntry(365) + ":\n";
        hos::PendingUpdateVersion pupd = {};
        bool pendingpresent = hos::GetPendingUpdateInfo(&pupd);
        auto pendfwver = hos::ConvertPendingUpdateVersion(pupd);
        if(pendingpresent)
        {
            msg += pu::String("\n" + set::GetDictionaryEntry(363) + ": ") + std::to_string(pupd.Major) + "." + std::to_string(pupd.Minor) + "." + std::to_string(pupd.Micro);
            msg += "\n" + set::GetDictionaryEntry(366);
        }
        else msg += "\n" + set::GetDictionaryEntry(367);

        std::vector<pu::String> opts = {set::GetDictionaryEntry(234), set::GetDictionaryEntry(368)};
        if(pendingpresent)
        {
            opts.push_back(set::GetDictionaryEntry(369));
            opts.push_back(set::GetDictionaryEntry(370));
        }

        auto sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(360), msg, opts, false);
        if(sopt == 0) return;
        else if(sopt == 1) this->HandleUpdate("Contents/registered", fwver);
        else if(sopt == 2)
        {
            auto sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(371), set::GetDictionaryEntry(372) + "\n" + set::GetDictionaryEntry(373), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt == 0)
            {
                hos::CleanPendingUpdate();
                mainapp->ShowNotification(set::GetDictionaryEntry(374));
            }
        }
        else if(sopt == 3) this->HandleUpdate("Contents/placehld", pendfwver);
    }

    void SettingsLayout::optsMemory_Click()
    {
        mainapp->GetMemoryLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetMemoryLayout());
    }
}