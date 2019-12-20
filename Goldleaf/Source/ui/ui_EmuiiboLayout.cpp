
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

#include <ui/ui_EmuiiboLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    EmuiiboLayout::EmuiiboLayout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, "Loading and querying amiibos...");
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(global_settings.custom_scheme.Text);
        this->optionsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optionsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optionsMenu);
        this->Add(this->optionsMenu);
    }

    void EmuiiboLayout::state_Click()
    {
        auto sopt = global_app->CreateShowDialog("State change", "Which state would you like to cfg?", { "On", "On (once)", "Off", "Cancel" }, true);
        if(sopt >= 0)
        {
            Result rc = 0;
            switch(sopt)
            {
                case 0:
                    rc = nfp::emu::SetEmulationOnForever();
                    break;
                case 1:
                    rc = nfp::emu::SetEmulationOnOnce();
                    break;
                case 2:
                    rc = nfp::emu::SetEmulationOff();
                    break;
                default:
                    return;
            }
            if(R_SUCCEEDED(rc))
            {
                global_app->ShowNotification("Amiibo emulation state was successfully cfg.");
                this->Reload();
            }
            else HandleResult(rc, "An error ocurred while attempting to cfg the amiibo emulation state:");
        }
    }
            
    void EmuiiboLayout::amiibo_Click(String path)
    {
        nfp::emu::EmulationStatus status = nfp::emu::EmulationStatus::Off;
        nfp::emu::GetStatus(&status);
        if(!nfp::emu::StatusIsOn(status))
        {
            global_app->ShowNotification("Amiibo emulation isn't activated.");
            return;
        }

        auto sopt = global_app->CreateShowDialog("Amiibo change", "Would you like to select this virtual amiibo?", { "Yes", "Cancel" }, true);
        if(sopt == 0)
        {
            auto rc = nfp::emu::SetCustomAmiibo(path.AsUTF8().c_str());
            if(R_SUCCEEDED(rc))
            {
                global_app->ShowNotification("The amiibo was successfully selected.");
                this->Reload();
            }
            else HandleResult(rc, "An error ocurred while attempting to select the amiibo:");
        }
    }

    void EmuiiboLayout::UpdateState()
    {
        nfp::emu::EmulationStatus status = nfp::emu::EmulationStatus::Off;
        auto rc = nfp::emu::GetStatus(&status);
        if(R_SUCCEEDED(rc))
        {
            switch(status)
            {
                case nfp::emu::EmulationStatus::Off:
                    global_app->LoadMenuHead("Amiibo emulation is off.");
                    break;
                case nfp::emu::EmulationStatus::OnForever:
                    global_app->LoadMenuHead("Amiibo emulation is on.");
                    break;
                case nfp::emu::EmulationStatus::OnOnce:
                    global_app->LoadMenuHead("Amiibo emulation is on (one single use).");
                    break;
            }
        }
    }

    void EmuiiboLayout::Reload()
    {
        this->infoText->SetVisible(true);
        this->optionsMenu->SetVisible(false);
        this->optionsMenu->ClearItems();
        global_app->CallForRender();
        if(nfp::emu::IsEmuiiboAccessible())
        {
            char curamiibo[FS_MAX_PATH] = {0};
            auto rc = nfp::emu::Initialize();
            if(R_SUCCEEDED(rc))
            {
                nfp::emu::Version v = {};
                auto rc = nfp::emu::GetVersion(&v);
                if(R_SUCCEEDED(rc))
                {
                    Version otherv = Version::MakeVersion(v.major, v.minor, v.micro);
                    if(otherv.IsLower(Version::MakeVersion(0, 4, 0))) global_app->ShowNotification("Only emuiibo 0.4.0 or newer is supported.");
                    else
                    {
                        auto sdex = fs::GetSdCardExplorer();
                        String emupath = "emuiibo/amiibo";
                        String sdemupath = "sdmc:/emuiibo/amiibo";
                        auto dirs = sdex->GetDirectories(emupath);

                        auto curamiiborc = nfp::emu::GetCurrentAmiibo(curamiibo, FS_MAX_PATH);

                        auto sitm = pu::ui::elm::MenuItem::New("Change emulation state");
                        sitm->SetColor(global_settings.custom_scheme.Text);
                        sitm->AddOnClick(std::bind(&EmuiiboLayout::state_Click, this));
                        this->optionsMenu->AddItem(sitm);

                        for(auto &emudir: dirs)
                        {
                            if(sdex->IsFile(emupath + "/" + emudir + "/model.json") && sdex->IsFile(emupath + "/" + emudir + "/tag.json") && sdex->IsFile(emupath + "/" + emudir + "/register.json") && sdex->IsFile(emupath + "/" + emudir + "/common.json"))
                            {
                                String amiibopath = sdemupath + "/" + emudir;
                                auto amiibo = nfp::emu::LoadVirtualAmiibo(amiibopath);
                                if(!amiibo.name.empty())
                                {
                                    String name = amiibo.name;
                                    if(R_SUCCEEDED(curamiiborc) && (strcasecmp(curamiibo, amiibopath.AsUTF8().c_str()) == 0))
                                    {
                                        name = "[Selected amiibo] ";
                                        name += amiibo.name;
                                    }
                                    auto aitm = pu::ui::elm::MenuItem::New(name);
                                    aitm->SetColor(global_settings.custom_scheme.Text);
                                    aitm->AddOnClick(std::bind(&EmuiiboLayout::amiibo_Click, this, amiibopath));
                                    auto img = nfp::emu::SaveAmiiboImageById(amiibo.id);
                                    if(img.HasAny()) aitm->SetIcon(img.AsUTF8());
                                    else aitm->SetIcon(global_settings.PathForResource("/Common/Amiibo.png"));
                                    this->optionsMenu->AddItem(aitm);
                                }
                            }
                        }
                    }
                }
            }
            if(R_FAILED(rc))
            {
                HandleResult(rc, "<emuiibo>");
                global_app->ReturnToMainMenu();
            }
            else
            {
                this->optionsMenu->SetSelectedIndex(0);
                this->infoText->SetVisible(false);
                this->optionsMenu->SetVisible(true);
            }
        }
        else
        {
            global_app->ShowNotification("emuiibo isn't present or loaded.");
            global_app->ReturnToMainMenu();
        }
    }
}