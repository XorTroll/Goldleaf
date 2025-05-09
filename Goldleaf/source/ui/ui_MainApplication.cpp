
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

    You should have received a copy_lyt of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_MainApplication.hpp>
#include <usb/usb_Base.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        bool g_WelcomeShown = false;

    }

    void MainApplication::DoLoadMenuData(const std::string &name, pu::sdl2::TextureHandle::Ref img, const std::string &temp_head) {
        if(this->menu_img != nullptr) {
            this->menu_img->SetVisible(true);
            this->menu_img->SetImage(img);
            this->menu_img->SetWidth(135);
            this->menu_img->SetHeight(135);
        }
        if(this->menu_name_text != nullptr) {
            this->menu_name_text->SetVisible(true);
            this->menu_name_text->SetText(name);
        }
        if(this->menu_head_text != nullptr) {
            this->menu_head_text->SetVisible(true);
            this->LoadMenuHead(temp_head);
        }
    }

    void MainApplication::helpImage_OnClick() {
        this->DisplayDialog(cfg::Strings.GetString(162), cfg::Strings.GetString(342) + "\n\n" + cfg::Strings.GetString(343) + "\n" + cfg::Strings.GetString(344) + "\n" + cfg::Strings.GetString(345) + "\n" + cfg::Strings.GetString(346) + "\n" + cfg::Strings.GetString(347), {cfg::Strings.GetString(234)}, false);
    }

    #define _UI_MAINAPP_MENU_SET_BASE(layout) { \
        layout->SetBackgroundColor(g_Settings.GetColorScheme().bg); \
        layout->Add(this->base_img); \
        layout->Add(this->time_text); \
        layout->Add(this->battery_text); \
        layout->Add(this->battery_img); \
        layout->Add(this->battery_charge_img); \
        layout->Add(this->menu_img); \
        layout->Add(this->usb_img); \
        layout->Add(this->conn_img); \
        layout->Add(this->ip_text); \
        layout->Add(this->menu_name_text); \
        layout->Add(this->menu_head_text); \
        layout->Add(this->user_img); \
        layout->Add(this->help_img); \
    }

    void MainApplication::OnLoad() {
        // Load the file hex-viewer font
        auto file_cnt_font = std::make_shared<pu::ttf::Font>(40);
        GLEAF_ASSERT_TRUE(file_cnt_font->LoadFromFile(g_Settings.PathForResource("/FileContentFont.ttf")));
        pu::ui::render::AddFont("FileContentFont", file_cnt_font);

        this->SetFadeAlphaIncrementStepCount(12);
        LoadCommonIcons();

        this->cur_battery_val = 0;
        this->cur_selected_user = {};
        this->cur_is_charging = false;
        this->cur_time = "";
        this->read_values_once = false;
        this->cur_conn_strength = 0;
        this->base_img = pu::ui::elm::Image::New(0, 0, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Base.png"))));
        this->time_text = pu::ui::elm::TextBlock::New(140, 32, "...");
        this->time_text->SetColor(g_Settings.GetColorScheme().text);
        this->battery_text = pu::ui::elm::TextBlock::New(1745, 32, "...");
        this->battery_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::MediumLarge));
        this->battery_text->SetColor(g_Settings.GetColorScheme().text);
        this->battery_img = pu::ui::elm::Image::New(1635, 10, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/0.png"))));
        this->battery_charge_img = pu::ui::elm::Image::New(1635, 10, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/Charge.png"))));
        this->battery_charge_img->SetWidth(80);
        this->battery_charge_img->SetHeight(80);
        this->menu_banner_img = pu::ui::elm::Image::New(40, 125, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/MenuBanner.png"))));
        this->menu_version_text = pu::ui::elm::TextBlock::New(590, 195, "v" GLEAF_VERSION);
        this->menu_version_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Large));
        this->menu_version_text->SetColor(g_Settings.GetColorScheme().version_text);
        this->menu_img = pu::ui::elm::Image::New(40, 125, GetCommonIcon(CommonIconKind::SdCard));
        this->user_img = ClickableImage::New(1600, 135, GetCommonIcon(CommonIconKind::User));
        this->user_img->SetWidth(105);
        this->user_img->SetHeight(105);
        this->user_img->SetOnClick(std::bind(&MainApplication::PickUser, this));
        this->help_img = ClickableImage::New(1770, 135, GetCommonIcon(CommonIconKind::Help));
        this->help_img->SetWidth(110);
        this->help_img->SetHeight(110);
        this->help_img->SetOnClick(std::bind(&MainApplication::helpImage_OnClick, this));
        this->usb_img = pu::ui::elm::Image::New(450, 10, GetCommonIcon(CommonIconKind::USB));
        this->usb_img->SetWidth(80);
        this->usb_img->SetHeight(80);
        this->usb_img->SetVisible(false);
        this->conn_img = pu::ui::elm::Image::New(640, 10, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Connection/None.png"))));
        this->conn_img->SetVisible(true);
        this->ip_text = pu::ui::elm::TextBlock::New(740, 32, "");
        this->ip_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::MediumLarge));
        this->ip_text->SetColor(g_Settings.GetColorScheme().text);
        this->menu_name_text = pu::ui::elm::TextBlock::New(200, 145, "...");
        this->menu_name_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Large));
        this->menu_name_text->SetColor(g_Settings.GetColorScheme().text);
        this->menu_head_text = pu::ui::elm::TextBlock::New(200, 200, "...");
        this->menu_head_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::MediumLarge));
        this->menu_head_text->SetColor(g_Settings.GetColorScheme().text);
        this->menu_head_text->SetClampWidth(1380);
        this->menu_head_text->SetClampSpeed(5);
        this->menu_head_text->SetClampDelay(60);
        this->UnloadMenuData();

        auto toast_text = pu::ui::elm::TextBlock::New(0, 0, "...");
        toast_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        toast_text->SetColor(pu::ui::Color(225, 225, 225, 255));
        toast_text->SetClampWidth(1720);
        toast_text->SetClampSpeed(10);
        toast_text->SetClampDelay(60);
        this->toast = pu::ui::extras::Toast::New(toast_text, pu::ui::Color(40, 40, 40, 255));

        this->UpdateValues();

        this->main_menu_lyt = MainMenuLayout::New();
        this->browser_lyt = BrowserLayout::New();
        this->file_content_lyt = FileContentLayout::New();
        this->copy_lyt = CopyLayout::New();
        this->explore_menu_lyt = ExploreMenuLayout::New();
        this->pc_explore_lyt = PCExploreLayout::New();
        this->install_lyt = InstallLayout::New();
        this->app_list_lyt = ApplicationListLayout::New();
        this->app_cnts_lyt = ApplicationContentsLayout::New();
        this->cnt_expt_lyt = ContentExportLayout::New();
        this->tiks_lyt = TicketsLayout::New();
        this->account_lyt = AccountLayout::New();
        this->amiibo_dump_lyt = AmiiboDumpLayout::New();
        this->settings_lyt = SettingsLayout::New();
        this->own_settings_lyt = OwnSettingsLayout::New();
        this->memory_lyt = MemoryLayout::New();
        this->update_lyt = UpdateLayout::New();
        this->update_install_lyt = UpdateInstallLayout::New();
        this->web_browser_lyt = WebBrowserLayout::New();
        this->about_lyt = AboutLayout::New();
        _UI_MAINAPP_MENU_SET_BASE(this->main_menu_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->browser_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->explore_menu_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->pc_explore_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->file_content_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->copy_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->install_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->app_list_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->app_cnts_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->cnt_expt_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->tiks_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->account_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->amiibo_dump_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->settings_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->own_settings_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->memory_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->update_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->update_install_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->web_browser_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->about_lyt);

        // Special extras
        this->main_menu_lyt->Add(this->menu_banner_img);
        this->main_menu_lyt->Add(this->menu_version_text);

        this->AddRenderCallback(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->ShowLayout(this->main_menu_lyt);
        this->start_time = std::chrono::steady_clock::now();
    }

    void MainApplication::ShowNotification(const std::string &text) {
        this->EndOverlay();
        this->toast->SetText(text);
        this->StartOverlayWithTimeout(this->toast, 2500);
    }

    int MainApplication::DisplayDialog(const std::string &title, const std::string &content, const std::vector<std::string> &opts, const bool use_last_opt_as_cancel, pu::sdl2::TextureHandle::Ref icon) {
        return this->CreateShowDialog(title, content, opts, use_last_opt_as_cancel, icon, [&](pu::ui::Dialog::Ref &dialog) {
            dialog->SetTitleColor(g_Settings.GetColorScheme().dialog_title);
            dialog->SetContentColor(g_Settings.GetColorScheme().dialog_title);
            dialog->SetOptionColor(g_Settings.GetColorScheme().dialog_opt);
            dialog->SetDialogColor(g_Settings.GetColorScheme().dialog);
            dialog->SetOverColor(g_Settings.GetColorScheme().dialog_over);
        });
    }

    void MainApplication::UpdateValues() {
        if(!g_WelcomeShown) {
            const auto time_now = std::chrono::steady_clock::now();
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - this->start_time).count();
            if(diff >= 1000) {
                this->ShowNotification(cfg::Strings.GetString(320));
                g_WelcomeShown = true;
            }
        }

        const auto cur_time = hos::GetCurrentTime(g_Settings.use_12h_time);
        const auto battery_val = hos::GetBatteryLevel();
        const auto is_charging = hos::IsCharging();
        if((this->cur_battery_val != battery_val) || !this->read_values_once) {
            if(battery_val <= 10) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/0.png"))));
            }
            else if((battery_val > 10) && (battery_val <= 20)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/10.png"))));
            }
            else if((battery_val > 20) && (battery_val <= 30)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/20.png"))));
            }
            else if((battery_val > 30) && (battery_val <= 40)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/30.png"))));
            }
            else if((battery_val > 40) && (battery_val <= 50)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/40.png"))));
            }
            else if((battery_val > 50) && (battery_val <= 60)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/50.png"))));
            }
            else if((battery_val > 60) && (battery_val <= 70)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/60.png"))));
            }
            else if((battery_val > 70) && (battery_val <= 80)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/70.png"))));
            }
            else if((battery_val > 80) && (battery_val <= 90)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/80.png"))));
            }
            else if((battery_val > 90) && (battery_val < 100)) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/90.png"))));
            }
            else if(battery_val == 100) {
                this->battery_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Battery/100.png"))));
            }
            this->battery_img->SetWidth(80);
            this->battery_img->SetHeight(80);
            this->battery_text->SetText(std::to_string(battery_val) + "%");
            this->cur_battery_val = battery_val;
        }
        if((this->cur_is_charging != is_charging) || !this->read_values_once) {
            this->battery_charge_img->SetVisible(is_charging);
            this->cur_is_charging = is_charging;
        }
        if((this->cur_time != cur_time) || !this->read_values_once)  {
            this->time_text->SetText(cur_time);
            this->cur_time = cur_time;
        }
        if(!this->read_values_once) {
            this->read_values_once = true;
        }

        this->usb_ok = usb::IsStateOk();
        this->usb_img->SetVisible(this->usb_ok);
        u32 conn_strength = 0;
        nifmGetInternetConnectionStatus(nullptr, &conn_strength, nullptr);
        if(conn_strength != this->cur_conn_strength) {
            std::string conn_img_name = "None";
            if(conn_strength > 0) {
                conn_img_name = std::to_string(conn_strength);
            }
            this->conn_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Connection/" + conn_img_name + ".png"))));
            this->conn_img->SetWidth(80);
            this->conn_img->SetHeight(80);
            this->cur_conn_strength = conn_strength;
        }

        char ip_str[0x20] = {};
        if(conn_strength > 0) {
            const auto ip = gethostid();
            inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str));
        }
        this->ip_text->SetText(ip_str);

        const auto selected_user = acc::GetSelectedUser();
        if(!acc::EqualUids(&selected_user, &this->cur_selected_user)) {
            this->cur_selected_user = selected_user;
            if(acc::HasSelectedUser()) {
                const auto user_icon = acc::GetExportedUserIcon();
                auto sd_exp = fs::GetSdCardExplorer();
                if(sd_exp->Exists(user_icon)) {
                    this->user_img->SetImage(pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(user_icon)));
                }
                else {
                    this->user_img->SetImage(GetCommonIcon(CommonIconKind::User));
                }
            }
            else {
                this->user_img->SetImage(GetCommonIcon(CommonIconKind::User));
            }
            this->user_img->SetWidth(105);
            this->user_img->SetHeight(105);
        }
    }

    void MainApplication::ReturnToParentLayout() {
        this->PopMenuData();
        this->PopLayout();
    }

    void MainApplication::LoadMenuData(const bool push_new, const std::string &name, pu::sdl2::TextureHandle::Ref icon, const std::string &temp_head) {
        if(push_new) {
            this->menu_data_stack.push({ name, icon, temp_head });
        }

        this->DoLoadMenuData(name, icon, temp_head);
    }

    void MainApplication::PopMenuData() {
        if(!this->menu_data_stack.empty()) {
            this->menu_data_stack.pop();
        }
        if(!this->menu_data_stack.empty()) {
            const auto &data = this->menu_data_stack.top();
            this->DoLoadMenuData(data.name, data.img, data.head);
        }
        else {
            this->UnloadMenuData();
        }
    }

    void MainApplication::LoadMenuHead(const std::string &head) {
        if(this->menu_head_text != nullptr) {
            this->menu_head_text->SetText(head);
        }
        if(!this->menu_data_stack.empty()) {
            this->menu_data_stack.top().head = head;
        }
    }

    void MainApplication::UnloadMenuData() {
        this->menu_img->SetVisible(false);
        this->menu_name_text->SetVisible(false);
        this->menu_head_text->SetVisible(false);
    }

    void MainApplication::ShowLayout(pu::ui::Layout::Ref lyt) {
        this->lyt_stack.push(lyt);
        this->LoadLayout(lyt);
    }

    void MainApplication::PopLayout() {
        if(!this->lyt_stack.empty()) {
            this->lyt_stack.pop();
        }
        if(!this->lyt_stack.empty()) {
            this->LoadLayout(this->lyt_stack.top());
        }
        else {
            GLEAF_ASSERT_FAIL("No layout to pop!");
        }
    }

    void MainApplication::ClearLayout(pu::ui::Layout::Ref lyt) {
        lyt->Clear();
        _UI_MAINAPP_MENU_SET_BASE(lyt);
    }

    void MainApplication::PickUser() {
        if(acc::SelectUser()) {
            acc::ExportSelectedUserIcon();
            this->ShowNotification(cfg::Strings.GetString(324));
        }
    }

    void MainApplication::OnInput(const u64 down, const u64 up, const u64 held) {
        if(!hos::IsExitLocked()) {
            if(down & HidNpadButton_Plus) {
                this->CloseWithFadeOut();
            }
            else if(down & HidNpadButton_Minus) {
                this->helpImage_OnClick();
            }
            else if((down & HidNpadButton_ZL) || (down & HidNpadButton_ZR)) {
                ShowPowerTasksDialog(cfg::Strings.GetString(229), cfg::Strings.GetString(230));
            }
        }
    }

    void UpdateClipboard(const std::string &path) {
        SetClipboard(path);
        g_MainApplication->ShowNotification(cfg::Strings.GetString(g_MainApplication->GetBrowserLayout()->GetExplorer()->IsFile(path) ? 257 : 258));
    }

}
