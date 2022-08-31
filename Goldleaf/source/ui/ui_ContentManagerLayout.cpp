
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    ContentManagerLayout::ContentManagerLayout() : pu::ui::Layout() {
        this->types_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->types_menu);
        this->sd_card_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(19));
        this->sd_card_menu_item->SetIcon(g_Settings.PathForResource("/Common/SdCard.png"));
        this->sd_card_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->sd_card_menu_item->AddOnKey(std::bind(&ContentManagerLayout::sdCardMenuItem_DefaultKey, this));
        this->nand_user_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(28));
        this->nand_user_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_user_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_user_menu_item->AddOnKey(std::bind(&ContentManagerLayout::nandUserMenuItem_DefaultKey, this));
        this->nand_system_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(29));
        this->nand_system_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_system_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_system_menu_item->AddOnKey(std::bind(&ContentManagerLayout::nandSystemMenuItem_DefaultKey, this));
        this->gamecart_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(190));
        this->gamecart_menu_item->SetIcon(g_Settings.PathForResource("/Common/GameCart.png"));
        this->gamecart_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->gamecart_menu_item->AddOnKey(std::bind(&ContentManagerLayout::gameCartMenuItem_DefaultKey, this));
        this->tickets_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(34));
        this->tickets_menu_item->SetIcon(g_Settings.PathForResource("/Common/Ticket.png"));
        this->tickets_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->tickets_menu_item->AddOnKey(std::bind(&ContentManagerLayout::ticketsMenuItem_DefaultKey, this));
        this->types_menu->AddItem(this->sd_card_menu_item);
        this->types_menu->AddItem(this->nand_user_menu_item);
        this->types_menu->AddItem(this->nand_system_menu_item);
        this->types_menu->AddItem(this->gamecart_menu_item);
        this->types_menu->AddItem(this->tickets_menu_item);
        this->Add(this->types_menu);
    }

    void ContentManagerLayout::sdCardMenuItem_DefaultKey() {
        g_MainApplication->GetStorageContentsLayout()->LoadFromStorage(NcmStorageId_SdCard);
        g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandUserMenuItem_DefaultKey() {
        g_MainApplication->GetStorageContentsLayout()->LoadFromStorage(NcmStorageId_BuiltInUser);
        g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandSystemMenuItem_DefaultKey() {
        g_MainApplication->GetStorageContentsLayout()->LoadFromStorage(NcmStorageId_BuiltInSystem);
        g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
    }

    void ContentManagerLayout::gameCartMenuItem_DefaultKey() {
        g_MainApplication->GetStorageContentsLayout()->LoadFromStorage(NcmStorageId_GameCard);
        g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
    }

    void ContentManagerLayout::ticketsMenuItem_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(34), "Ticket", cfg::strings::Main.GetString(35));
        g_MainApplication->GetTicketsLayout()->UpdateElements(true);
        g_MainApplication->LoadLayout(g_MainApplication->GetTicketsLayout());
    }

}