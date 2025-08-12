
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#include <ui/ui_ApplicationListLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void ApplicationListLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    ApplicationListLayout::ApplicationListLayout() : needs_menu_reload(true) {
        this->apps_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->apps_menu);
        this->no_apps_text = pu::ui::elm::TextBlock::New(0, 0, cfg::Strings.GetString(188));
        this->no_apps_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->no_apps_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->no_apps_text->SetColor(g_Settings.GetColorScheme().text);
        this->no_apps_text->SetVisible(false);
        this->Add(this->no_apps_text);
        this->Add(this->apps_menu);

        this->SetOnInput(std::bind(&ApplicationListLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void ApplicationListLayout::apps_DefaultKey(const u32 app_idx) {
        g_MainApplication->GetApplicationContentsLayout()->LoadApplication(app_idx);
        g_MainApplication->ShowLayout(g_MainApplication->GetApplicationContentsLayout());
    }

    void ApplicationListLayout::ReloadApplications() {
        if(!this->needs_menu_reload) {
            return;
        }
        this->needs_menu_reload = false;

        this->apps_menu->ClearItems();

        auto &apps = cnt::GetApplications();
        ClearApplicationIcons();
        u32 app_i = 0;
        for(auto &app: apps) {
            auto itm = pu::ui::elm::MenuItem::New(app.cache.display_name);
            itm->SetColor(g_Settings.GetColorScheme().text);
            if(app.HasIcon()) {
                auto app_icon = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromBuffer(app.metadata.icon_data, app.metadata.icon_size));
                SetApplicationIcon(app.record.id, app_icon);
                itm->SetIcon(app_icon);
            }
            itm->AddOnKey(std::bind(&ApplicationListLayout::apps_DefaultKey, this, app_i));
            this->apps_menu->AddItem(itm);
            app_i++;
        }

        const auto is_empty = apps.empty();
        this->no_apps_text->SetVisible(is_empty);
        this->apps_menu->SetVisible(!is_empty);
    }

    void ApplicationListLayout::Reload() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(488), CommonIconKind::Game, cfg::Strings.GetString(489));
        this->apps_menu->SetSelectedIndex(0);

        this->ReloadApplications();
    }

}
