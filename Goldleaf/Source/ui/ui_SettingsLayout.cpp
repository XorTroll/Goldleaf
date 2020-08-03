
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

#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    SettingsLayout::SettingsLayout() : pu::ui::Layout()
    {
        this->optsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optsMenu);
        auto itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(352));
        itm->SetColor(global_settings.custom_scheme.Text);
        itm->AddOnClick(std::bind(&SettingsLayout::optsFirmware_Click, this));
        this->optsMenu->AddItem(itm);
        auto itm2 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(353));
        itm2->SetColor(global_settings.custom_scheme.Text);
        itm2->AddOnClick(std::bind(&SettingsLayout::optsMemory_Click, this));
        this->optsMenu->AddItem(itm2);
        auto itm3 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(354));
        itm3->SetColor(global_settings.custom_scheme.Text);
        itm3->AddOnClick(std::bind(&SettingsLayout::optsConfig_Click, this));
        this->optsMenu->AddItem(itm3);
        this->progressInfo = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->progressInfo->SetVisible(false);
        global_settings.ApplyProgressBarColor(this->progressInfo);
        this->Add(this->optsMenu);
        this->Add(this->progressInfo);
    }

    void SettingsLayout::optsConfig_Click()
    {
        String msg = cfg::strings::Main.GetString(354) + ":\n";
        msg += String("\n" + cfg::strings::Main.GetString(355) + ": ") + (global_settings.ignore_required_fw_ver ? cfg::strings::Main.GetString(111) : cfg::strings::Main.GetString(112));
        if(!global_settings.external_romfs.empty()) msg += "\n" + cfg::strings::Main.GetString(356) + ": 'SdCard:/" + global_settings.external_romfs + "'";
        global_app->CreateShowDialog(cfg::strings::Main.GetString(357), msg, { cfg::strings::Main.GetString(234) }, true);
    }

    void SettingsLayout::ExportUpdateToDirectory(String Input, SetSysFirmwareVersion Fw)
    {
        auto sd = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->optsMenu->SetVisible(false);
        this->progressInfo->SetVisible(true);
        global_app->LoadMenuHead(cfg::strings::Main.GetString(359) + " " + Fw.display_version + "...");
        auto outdir = sd->FullPathFor(consts::Root + "/dump/update/" + Fw.display_version);
        sd->DeleteDirectory(outdir);
        exp->CopyDirectoryProgress(Input, outdir, [&](double Done, double Total)
        {
            this->progressInfo->SetMaxValue(Total);
            this->progressInfo->SetProgress(Done);
            global_app->CallForRender();
        });
        // Rename meta NCAs to .cnmt.nca so that this update is valid for Daybreak
        auto files = sd->GetFiles(outdir);
        for(auto file: files)
        {
            auto original_nca = "@SystemContent:/registered/" + file;
            FsFileSystem nca_fs;
            auto rc = fsOpenFileSystemWithId(&nca_fs, 0, FsFileSystemType_ContentMeta, original_nca.AsUTF8().c_str());
            if(R_SUCCEEDED(rc))
            {
                // Is a meta NCA
                auto out_nca_path = outdir + "/" + file;
                auto out_nca_path_noext = out_nca_path.substr(0, out_nca_path.length() - 3);
                auto out_cnmt_nca_path = out_nca_path_noext + "cnmt.nca";
                sd->RenameFile(out_nca_path, out_cnmt_nca_path);
            }
        }
        global_app->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
        this->optsMenu->SetVisible(true);
        this->progressInfo->SetVisible(false);
        global_app->ShowNotification(cfg::strings::Main.GetString(358) + " '" + outdir + "'.");
    }

    void SettingsLayout::ExportUpdateToNSP(String Input, SetSysFirmwareVersion Fw)
    {
        auto sd = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->optsMenu->SetVisible(false);
        this->progressInfo->SetVisible(true);
        global_app->LoadMenuHead(cfg::strings::Main.GetString(359) + " " + Fw.display_version + "...");
        auto outnsp = sd->FullPathFor(consts::Root + "/dump/update/" + Fw.display_version + ".nsp");
        sd->DeleteFile(outnsp);
        nsp::GenerateFrom(exp->FullPathFor(Input), outnsp, [&](u64 Done, u64 Total)
        {
            this->progressInfo->SetMaxValue((double)Total);
            this->progressInfo->SetProgress((double)Done);
            global_app->CallForRender();
        });
        global_app->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
        this->optsMenu->SetVisible(true);
        this->progressInfo->SetVisible(false);
        global_app->ShowNotification(cfg::strings::Main.GetString(358) + " '" + outnsp + "'.");
    }

    void SettingsLayout::HandleUpdate(String Base, SetSysFirmwareVersion Fw)
    {
        auto sopt2 = global_app->CreateShowDialog(cfg::strings::Main.GetString(360), cfg::strings::Main.GetString(361), { cfg::strings::Main.GetString(377), cfg::strings::Main.GetString(53), cfg::strings::Main.GetString(18)}, true);
        if(sopt2 == 0) ExportUpdateToDirectory(Base, Fw);
        else if(sopt2 == 1) ExportUpdateToNSP(Base, Fw);
    }

    void SettingsLayout::optsFirmware_Click()
    {
        SetSysFirmwareVersion fwver = {};
        setsysGetFirmwareVersion(&fwver);
        String msg = cfg::strings::Main.GetString(362) + ":\n";
        msg += String("\n" + cfg::strings::Main.GetString(363) + ": ") + fwver.display_version + " (" + fwver.display_title + ")";
        msg += String("\n" + cfg::strings::Main.GetString(364) + ": '") + fwver.version_hash + "'";
        msg += String("\n" + cfg::strings::Main.GetString(95) + " ") + std::to_string(hos::ComputeSystemKeyGeneration());
        msg += "\n\n" + cfg::strings::Main.GetString(365) + ":\n";
        hos::PendingUpdateVersion pupd = {};
        bool pendingpresent = hos::GetPendingUpdateInfo(&pupd);
        auto pendfwver = hos::ConvertPendingUpdateVersion(pupd);
        if(pendingpresent)
        {
            msg += String("\n" + cfg::strings::Main.GetString(363) + ": ") + std::to_string(pupd.Major) + "." + std::to_string(pupd.Minor) + "." + std::to_string(pupd.Micro);
            msg += "\n" + cfg::strings::Main.GetString(366);
        }
        else msg += "\n" + cfg::strings::Main.GetString(367);

        std::vector<String> opts = {cfg::strings::Main.GetString(234), cfg::strings::Main.GetString(368)};
        if(pendingpresent)
        {
            opts.push_back(cfg::strings::Main.GetString(369));
            opts.push_back(cfg::strings::Main.GetString(370));
        }

        auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(360), msg, opts, false);
        if(sopt == 0) return;
        else if(sopt == 1) this->HandleUpdate("Contents/registered", fwver);
        else if(sopt == 2)
        {
            auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(371), cfg::strings::Main.GetString(372) + "\n" + cfg::strings::Main.GetString(373), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(sopt == 0)
            {
                hos::CleanPendingUpdate();
                global_app->ShowNotification(cfg::strings::Main.GetString(374));
            }
        }
        else if(sopt == 3) this->HandleUpdate("Contents/placehld", pendfwver);
    }

    void SettingsLayout::optsMemory_Click()
    {
        global_app->GetMemoryLayout()->UpdateElements();
        global_app->LoadLayout(global_app->GetMemoryLayout());
    }
}