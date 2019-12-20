
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

#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    MainMenuLayout::MainMenuLayout() : pu::ui::Layout()
    {
        this->optionMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optionMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optionMenu);
        this->exploreMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(277));
        this->exploreMenuItem->SetIcon(global_settings.PathForResource("/Common/SdCard.png"));
        this->exploreMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->exploreMenuItem->AddOnClick(std::bind(&MainMenuLayout::exploreMenuItem_Click, this));
        this->titleMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(3));
        this->titleMenuItem->SetIcon(global_settings.PathForResource("/Common/Storage.png"));
        this->titleMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->webMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(5));
        this->webMenuItem->SetIcon(global_settings.PathForResource("/Common/Browser.png"));
        this->webMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->webMenuItem->AddOnClick(std::bind(&MainMenuLayout::webMenuItem_Click, this));
        this->accountMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(6));
        this->accountMenuItem->SetIcon(global_settings.PathForResource("/Common/Accounts.png"));
        this->accountMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->accountMenuItem->AddOnClick(std::bind(&MainMenuLayout::accountMenuItem_Click, this));
        this->amiiboMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(283));
        this->amiiboMenuItem->SetIcon(global_settings.PathForResource("/Common/Amiibo.png"));
        this->amiiboMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->amiiboMenuItem->AddOnClick(std::bind(&MainMenuLayout::amiiboMenuItem_Click, this));
        this->emuiiboMenuItem = pu::ui::elm::MenuItem::New("emuiibo management");
        this->emuiiboMenuItem->SetIcon(global_settings.PathForResource("/Common/Amiibo.png"));
        this->emuiiboMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->emuiiboMenuItem->AddOnClick(std::bind(&MainMenuLayout::emuiiboMenuItem_Click, this));
        this->settingsMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(375));
        this->settingsMenuItem->SetIcon(global_settings.PathForResource("/Common/Settings.png"));
        this->settingsMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->settingsMenuItem->AddOnClick(std::bind(&MainMenuLayout::settingsMenuItem_Click, this));
        this->updateMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(284));
        this->updateMenuItem->SetIcon(global_settings.PathForResource("/Common/Update.png"));
        this->updateMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->updateMenuItem->AddOnClick(std::bind(&MainMenuLayout::updateMenuItem_Click, this));
        this->aboutMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(8));
        this->aboutMenuItem->SetIcon(global_settings.PathForResource("/Common/Info.png"));
        this->aboutMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->aboutMenuItem->AddOnClick(std::bind(&MainMenuLayout::aboutMenuItem_Click, this));
        this->optionMenu->AddItem(this->exploreMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->webMenuItem);
        this->optionMenu->AddItem(this->accountMenuItem);
        this->optionMenu->AddItem(this->amiiboMenuItem);
        this->optionMenu->AddItem(this->emuiiboMenuItem);
        this->optionMenu->AddItem(this->settingsMenuItem);
        this->optionMenu->AddItem(this->updateMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->Add(this->optionMenu);
    }

    void MainMenuLayout::exploreMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(277), "Storage", cfg::strings::Main.GetString(278));
        global_app->LoadLayout(global_app->GetExploreMenuLayout());
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(32), "Storage", cfg::strings::Main.GetString(33));
        EnsureDirectories();
        global_app->LoadLayout(global_app->GetContentManagerLayout());
    }

    void MainMenuLayout::webMenuItem_Click()
    {
        global_app->LoadMenuData("Web browser", "Browser", "Use the console's web browser library applet.");
        global_app->GetWebBrowserLayout()->Refresh();
        global_app->LoadLayout(global_app->GetWebBrowserLayout());
    }

    void MainMenuLayout::accountMenuItem_Click()
    {
        if(!acc::HasUser())
        {
            int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(348), cfg::strings::Main.GetString(349), {cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18)}, true);
            if(sopt != 0) return;
            if(acc::SelectUser()) global_app->ShowNotification(cfg::strings::Main.GetString(324));
            else
            {
                global_app->ShowNotification(cfg::strings::Main.GetString(350));
                return;
            }
        }
        global_app->LoadMenuData(cfg::strings::Main.GetString(41), "Accounts", cfg::strings::Main.GetString(42));
        global_app->GetAccountLayout()->Load();
        global_app->LoadLayout(global_app->GetAccountLayout());
    }

    void MainMenuLayout::amiiboMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(283), "Amiibo", cfg::strings::Main.GetString(301));
        global_app->LoadLayout(global_app->GetAmiiboDumpLayout());
        global_app->GetAmiiboDumpLayout()->StartDump();
        global_app->ReturnToMainMenu();
    }

    void MainMenuLayout::emuiiboMenuItem_Click()
    {
        global_app->LoadMenuData("emuiibo manager", "Emuiibo", "Loading status...");
        global_app->LoadLayout(global_app->GetEmuiiboLayout());
        global_app->GetEmuiiboLayout()->Reload();
    }

    void MainMenuLayout::settingsMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(375), "Settings", cfg::strings::Main.GetString(376));
        global_app->LoadLayout(global_app->GetSettingsLayout());
    }

    void MainMenuLayout::updateMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(284), "Update", cfg::strings::Main.GetString(302));
        global_app->LoadLayout(global_app->GetUpdateLayout());
        global_app->GetUpdateLayout()->StartUpdateSearch();
    }

    void MainMenuLayout::aboutMenuItem_Click()
    {
        String exmode = cfg::strings::Main.GetString(288);
        if(GetExecutableMode() == ExecutableMode::NSO) exmode = cfg::strings::Main.GetString(289);

        String lmode;
        if(GetLaunchMode() == LaunchMode::Applet) lmode = cfg::strings::Main.GetString(290);
        if(GetLaunchMode() == LaunchMode::Application) lmode = cfg::strings::Main.GetString(291);

        global_app->LoadMenuData("Goldleaf v" + GetVersion(), "Info", exmode.AsUTF8() + ", " + lmode.AsUTF8());
        global_app->LoadLayout(global_app->GetAboutLayout());
    }
}