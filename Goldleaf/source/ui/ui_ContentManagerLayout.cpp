
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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
        this->sd_card_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(19));
        this->sd_card_menu_item->SetIcon(g_Settings.PathForResource("/Common/SdCard.png"));
        this->sd_card_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->sd_card_menu_item->AddOnKey(std::bind(&ContentManagerLayout::storageMenuItem_DefaultKey, this, NcmStorageId_SdCard));
        this->nand_user_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(28));
        this->nand_user_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_user_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_user_menu_item->AddOnKey(std::bind(&ContentManagerLayout::storageMenuItem_DefaultKey, this, NcmStorageId_BuiltInUser));
        this->nand_system_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(29));
        this->nand_system_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_system_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_system_menu_item->AddOnKey(std::bind(&ContentManagerLayout::storageMenuItem_DefaultKey, this, NcmStorageId_BuiltInSystem));
        this->gamecard_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(190));
        this->gamecard_menu_item->SetIcon(g_Settings.PathForResource("/Common/GameCard.png"));
        this->gamecard_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->gamecard_menu_item->AddOnKey(std::bind(&ContentManagerLayout::storageMenuItem_DefaultKey, this, NcmStorageId_GameCard));
        this->other_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(466));
        this->other_menu_item->SetIcon(g_Settings.PathForResource("/Common/Help.png"));
        this->other_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->other_menu_item->AddOnKey(std::bind(&ContentManagerLayout::storageMenuItem_DefaultKey, this, NcmStorageId_None));
        this->tickets_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(34));
        this->tickets_menu_item->SetIcon(g_Settings.PathForResource("/Common/Ticket.png"));
        this->tickets_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->tickets_menu_item->AddOnKey(std::bind(&ContentManagerLayout::ticketsMenuItem_DefaultKey, this));
        this->types_menu->AddItem(this->sd_card_menu_item);
        this->types_menu->AddItem(this->nand_user_menu_item);
        this->types_menu->AddItem(this->nand_system_menu_item);
        this->types_menu->AddItem(this->gamecard_menu_item);
        this->types_menu->AddItem(this->other_menu_item);
        this->types_menu->AddItem(this->tickets_menu_item);
        this->Add(this->types_menu);
    }

    void ContentManagerLayout::storageMenuItem_DefaultKey(const NcmStorageId storage_id) {
        g_MainApplication->GetStorageContentsLayout()->LoadFromStorage(storage_id);
        g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
    }

    void ContentManagerLayout::ticketsMenuItem_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::Strings.GetString(34), "Ticket", cfg::Strings.GetString(35));
        g_MainApplication->GetTicketsLayout()->UpdateElements(true);
        g_MainApplication->LoadLayout(g_MainApplication->GetTicketsLayout());
    }

}