
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

#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void AboutLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
        else if(keys_down & HidNpadButton_A) {
            g_MainApplication->ShowLayout(g_MainApplication->GetUpdateLayout());
            g_MainApplication->GetUpdateLayout()->StartUpdateSearch();
        }
    }

    AboutLayout::AboutLayout() {
        this->logo_img = pu::ui::elm::Image::New(448, 324, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Logo.png"))));
        this->Add(this->logo_img);

        this->update_info_text = pu::ui::elm::TextBlock::New(0, 850, cfg::Strings.GetString(487));
        this->update_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->update_info_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::MediumLarge));
        this->update_info_text->SetColor(g_Settings.GetColorScheme().text);
        this->Add(this->update_info_text);

        this->SetOnInput(std::bind(&AboutLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void AboutLayout::Load() {
        const auto exec_mode_str = cfg::Strings.GetString((GetExecutableMode() == ExecutableMode::NRO) ? 288 : 289);
        const auto launch_mode_str = cfg::Strings.GetString((GetLaunchMode() == LaunchMode::Applet) ? 290 : 291);
        g_MainApplication->LoadCommonIconMenuData(true, "Goldleaf v" GLEAF_VERSION, CommonIconKind::Info, exec_mode_str + ", " + launch_mode_str);
    }

}
