
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

#include <ui/ui_ContentManagerLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    ContentManagerLayout::ContentManagerLayout() : pu::ui::Layout()
    {
        this->typesMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->typesMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->typesMenu);
        this->sdCardMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(19));
        this->sdCardMenuItem->SetIcon(global_settings.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ContentManagerLayout::sdCardMenuItem_Click, this));
        this->nandUserMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(28));
        this->nandUserMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandUserMenuItem_Click, this));
        this->nandSystemMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(29));
        this->nandSystemMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandSystemMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandSystemMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandSystemMenuItem_Click, this));
        this->gameCartMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(190));
        this->gameCartMenuItem->SetIcon(global_settings.PathForResource("/Common/GameCart.png"));
        this->gameCartMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->gameCartMenuItem->AddOnClick(std::bind(&ContentManagerLayout::gameCartMenuItem_Click, this));
        this->unusedTicketsMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(287));
        this->unusedTicketsMenuItem->SetIcon(global_settings.PathForResource("/Common/Ticket.png"));
        this->unusedTicketsMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->unusedTicketsMenuItem->AddOnClick(std::bind(&ContentManagerLayout::unusedTicketsMenuItem_Click, this));
        this->typesMenu->AddItem(this->sdCardMenuItem);
        this->typesMenu->AddItem(this->nandUserMenuItem);
        this->typesMenu->AddItem(this->nandSystemMenuItem);
        this->typesMenu->AddItem(this->gameCartMenuItem);
        this->typesMenu->AddItem(this->unusedTicketsMenuItem);
        this->Add(this->typesMenu);
    }

    void ContentManagerLayout::sdCardMenuItem_Click()
    {
        global_app->GetStorageContentsLayout()->LoadFromStorage(Storage::SdCard);
        global_app->LoadLayout(global_app->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandUserMenuItem_Click()
    {
        global_app->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDUser);
        global_app->LoadLayout(global_app->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandSystemMenuItem_Click()
    {
        global_app->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDSystem);
        global_app->LoadLayout(global_app->GetStorageContentsLayout());
    }

    void ContentManagerLayout::gameCartMenuItem_Click()
    {
        global_app->GetStorageContentsLayout()->LoadFromStorage(Storage::GameCart);
        global_app->LoadLayout(global_app->GetStorageContentsLayout());
    }

    void ContentManagerLayout::unusedTicketsMenuItem_Click()
    {
        global_app->LoadMenuData(cfg::strings::Main.GetString(34), "Ticket", cfg::strings::Main.GetString(35));
        global_app->GetUnusedTicketsLayout()->UpdateElements(true);
        global_app->LoadLayout(global_app->GetUnusedTicketsLayout());
    }
}