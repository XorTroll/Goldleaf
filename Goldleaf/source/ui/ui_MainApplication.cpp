
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

    You should have received a copy_lyt of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_MainApplication.hpp>
#include <usb/usb_Base.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    extern std::string g_Clipboard;

    namespace {

        bool g_WelcomeShown = false;

    }

    #define _UI_MAINAPP_MENU_SET_BASE(layout) { \
        layout->SetBackgroundColor(g_Settings.custom_scheme.bg); \
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

    // TODO (low priority): move OnInputs to each layout's code?

    void MainApplication::OnLoad() {
        // Load the file hex-viewer font
        pu::ui::render::AddFontFile("FileContentFont", 25, g_Settings.PathForResource("/FileSystem/FileContentFont.ttf"));

        this->cur_battery_val = 0;
        this->cur_selected_user = {};
        this->cur_is_charging = false;
        this->cur_time = "";
        this->read_values_once = false;
        this->cur_conn_strength = 0;
        this->base_img = pu::ui::elm::Image::New(0, 0, g_Settings.PathForResource("/Base.png"));
        this->time_text = pu::ui::elm::TextBlock::New(1124, 18, "...");
        this->time_text->SetColor(g_Settings.custom_scheme.text);
        this->battery_text = pu::ui::elm::TextBlock::New(1015, 22, "...");
        this->battery_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->battery_text->SetColor(g_Settings.custom_scheme.text);
        this->battery_img = pu::ui::elm::Image::New(960, 8, g_Settings.PathForResource("/Battery/0.png"));
        this->battery_charge_img = pu::ui::elm::Image::New(960, 8, g_Settings.PathForResource("/Battery/Charge.png"));
        this->menu_banner_img = pu::ui::elm::Image::New(10, 62, g_Settings.PathForResource("/MenuBanner.png"));
        this->menu_img = pu::ui::elm::Image::New(15, 69, g_Settings.PathForResource("/Common/SdCard.png"));
        this->menu_img->SetWidth(85);
        this->menu_img->SetHeight(85);
        this->user_img = ClickableImage::New(1090, 75, g_Settings.PathForResource("/Common/User.png"));
        this->user_img->SetWidth(70);
        this->user_img->SetHeight(70);
        this->user_img->SetOnClick(std::bind(&MainApplication::userImage_OnClick, this));
        this->help_img = ClickableImage::New(1180, 80, g_Settings.PathForResource("/Common/Help.png"));
        this->help_img->SetWidth(60);
        this->help_img->SetHeight(60);
        this->help_img->SetOnClick(std::bind(&MainApplication::helpImage_OnClick, this));
        this->usb_img = pu::ui::elm::Image::New(695, 12, g_Settings.PathForResource("/Common/USB.png"));
        this->usb_img->SetWidth(40);
        this->usb_img->SetHeight(40);
        this->usb_img->SetVisible(false);
        this->conn_img = pu::ui::elm::Image::New(755, 12, g_Settings.PathForResource("/Connection/None.png"));
        this->conn_img->SetWidth(40);
        this->conn_img->SetHeight(40);
        this->conn_img->SetVisible(true);
        this->ip_text = pu::ui::elm::TextBlock::New(800, 22, "");
        this->ip_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->ip_text->SetColor(g_Settings.custom_scheme.text);
        this->menu_name_text = pu::ui::elm::TextBlock::New(120, 85, "...");
        this->menu_name_text->SetColor(g_Settings.custom_scheme.text);
        this->menu_head_text = pu::ui::elm::TextBlock::New(120, 120, "...");
        this->menu_head_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->menu_head_text->SetColor(g_Settings.custom_scheme.text);
        this->UnloadMenuData();
        this->toast = pu::ui::extras::Toast::New("...", pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium), pu::ui::Color(225, 225, 225, 255), pu::ui::Color(40, 40, 40, 255));
        this->UpdateValues();
        this->main_menu_lyt = MainMenuLayout::New();
        this->partition_browser_lyt = PartitionBrowserLayout::New();
        this->partition_browser_lyt->SetOnInput(std::bind(&MainApplication::browser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->file_content_lyt = FileContentLayout::New();
        this->file_content_lyt->SetOnInput(std::bind(&MainApplication::fileContent_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->copy_lyt = CopyLayout::New();
        this->explore_menu_lyt = ExploreMenuLayout::New();
        this->explore_menu_lyt->SetOnInput(std::bind(&MainApplication::exploreMenu_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->pc_explore_lyt = PCExploreLayout::New();
        this->pc_explore_lyt->SetOnInput(std::bind(&MainApplication::pcExplore_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->install_lyt = InstallLayout::New();
        this->cnt_information_lyt = ContentInformationLayout::New();
        this->cnt_information_lyt->SetOnInput(std::bind(&MainApplication::contentInformation_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->storage_cnts_lyt = StorageContentsLayout::New();
        this->storage_cnts_lyt->SetOnInput(std::bind(&MainApplication::storageContents_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->cnt_manager_lyt = ContentManagerLayout::New();
        this->cnt_manager_lyt->SetOnInput(std::bind(&MainApplication::contentManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->cnt_expt_lyt = ContentExportLayout::New();
        this->tiks_lyt = TicketsLayout::New();
        this->tiks_lyt->SetOnInput(std::bind(&MainApplication::unusedTickets_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->account_lyt = AccountLayout::New();
        this->account_lyt->SetOnInput(std::bind(&MainApplication::account_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->amiibo_dump_lyt = AmiiboDumpLayout::New();
        this->amiibo_dump_lyt->SetOnInput(std::bind(&MainApplication::amiibo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->settings_lyt = SettingsLayout::New();
        this->settings_lyt->SetOnInput(std::bind(&MainApplication::settings_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->own_settings_lyt = OwnSettingsLayout::New();
        this->own_settings_lyt->SetOnInput(std::bind(&MainApplication::ownSettings_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->memory_lyt = MemoryLayout::New();
        this->memory_lyt->SetOnInput(std::bind(&MainApplication::memory_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->update_lyt = UpdateLayout::New();
        this->update_install_lyt = UpdateInstallLayout::New();
        this->web_browser_lyt = WebBrowserLayout::New();
        this->web_browser_lyt->SetOnInput(std::bind(&MainApplication::webBrowser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->about_lyt = AboutLayout::New();
        this->about_lyt->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        _UI_MAINAPP_MENU_SET_BASE(this->main_menu_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->partition_browser_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->explore_menu_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->pc_explore_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->file_content_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->copy_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->install_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->cnt_information_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->storage_cnts_lyt);
        _UI_MAINAPP_MENU_SET_BASE(this->cnt_manager_lyt);
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

        this->AddRenderCallback(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->LoadLayout(this->main_menu_lyt);
        this->start_time = std::chrono::steady_clock::now();
    }

    void MainApplication::ShowNotification(const std::string &text) {
        this->EndOverlay();
        this->toast->SetText(text);
        this->StartOverlayWithTimeout(this->toast, 1500);
    }

    void MainApplication::UpdateValues() {
        if(!g_WelcomeShown) {
            const auto time_now = std::chrono::steady_clock::now();
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - this->start_time).count();
            if(diff >= 1000) {
                this->ShowNotification(cfg::strings::Main.GetString(320));
                g_WelcomeShown = true;
            }
        }

        const auto cur_time = hos::GetCurrentTime();
        const auto battery_val = hos::GetBatteryLevel();
        const auto is_charging = hos::IsCharging();
        if((this->cur_battery_val != battery_val) || !this->read_values_once) {
            if(battery_val <= 10) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/0.png"));
            }
            else if((battery_val > 10) && (battery_val <= 20)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/10.png"));
            }
            else if((battery_val > 20) && (battery_val <= 30)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/20.png"));
            }
            else if((battery_val > 30) && (battery_val <= 40)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/30.png"));
            }
            else if((battery_val > 40) && (battery_val <= 50)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/40.png"));
            }
            else if((battery_val > 50) && (battery_val <= 60)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/50.png"));
            }
            else if((battery_val > 60) && (battery_val <= 70)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/60.png"));
            }
            else if((battery_val > 70) && (battery_val <= 80)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/70.png"));
            }
            else if((battery_val > 80) && (battery_val <= 90)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/80.png"));
            }
            else if((battery_val > 90) && (battery_val < 100)) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/90.png"));
            }
            else if(battery_val == 100) {
                this->battery_img->SetImage(g_Settings.PathForResource("/Battery/100.png"));
            }
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
            this->conn_img->SetImage(g_Settings.PathForResource("/Connection/" + conn_img_name + ".png"));
            this->conn_img->SetWidth(40);
            this->conn_img->SetHeight(40);
            this->cur_conn_strength = conn_strength;
        }
        if(conn_strength > 0) {
            const auto ip = gethostid();
            char ip_str[0x20] = {0};
            inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str));
            this->ip_text->SetText(ip_str);
        }
        else {
            this->ip_text->SetText("");
        }
        const auto selected_user = acc::GetSelectedUser();
        if(!acc::UidCompare(&selected_user, &this->cur_selected_user)) {
            this->cur_selected_user = selected_user;
            if(acc::HasSelectedUser()) {
                const auto user_icon = acc::GetCachedUserIcon();
                auto sd_exp = fs::GetSdCardExplorer();
                if(sd_exp->Exists(user_icon)) {
                    this->user_img->SetImage(user_icon);
                }
                else {
                    this->user_img->SetImage(g_Settings.PathForResource("/Common/User.png"));
                }
            }
            else {
                this->user_img->SetImage(g_Settings.PathForResource("/Common/User.png"));
            }
            this->user_img->SetWidth(70);
            this->user_img->SetHeight(70);
        }
    }

    void MainApplication::ReturnToMainMenu() {
        this->UnloadMenuData();
        this->LoadLayout(this->main_menu_lyt);
    }

    void MainApplication::LoadMenuData(const std::string &name, const std::string &img_name, const std::string &temp_head, const bool is_common_icon) {
        if(this->menu_img != nullptr) {
            this->menu_img->SetVisible(true);
            if(is_common_icon) {
                this->menu_img->SetImage(g_Settings.PathForResource("/Common/" + img_name + ".png"));
            }
            else {
                this->menu_img->SetImage(img_name);
            }
            this->menu_img->SetWidth(85);
            this->menu_img->SetHeight(85);
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

    void MainApplication::LoadMenuHead(const std::string &head) {
        if(this->menu_head_text != nullptr) {
            this->menu_head_text->SetText(head);
        }
    }

    void MainApplication::UnloadMenuData() {
        this->menu_img->SetVisible(false);
        this->menu_name_text->SetVisible(false);
        this->menu_head_text->SetVisible(false);
    }

    void MainApplication::browser_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            if(this->partition_browser_lyt->GoBack()) {
                this->partition_browser_lyt->UpdateElements(-1);
            }
            else {
                this->UnloadMenuData();
                this->LoadMenuData(cfg::strings::Main.GetString(277), "Storage", cfg::strings::Main.GetString(278));
                this->LoadLayout(this->explore_menu_lyt);
            }
        }
        else if(down & HidNpadButton_X) {
            if(!g_Clipboard.empty()) {
                auto exp = fs::GetExplorerForPath(g_Clipboard);
                const bool clipboard_is_dir = exp->IsDirectory(g_Clipboard);
                std::string fs_icon;
                if(clipboard_is_dir) {
                    fs_icon = g_Settings.PathForResource("/FileSystem/Directory.png");
                }
                else {
                    auto ext = fs::GetExtension(g_Clipboard);
                    if(ext == "nsp") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/NSP.png");
                    }
                    else if(ext == "nro") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/NRO.png");
                    }
                    else if(ext == "tik") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/TIK.png");
                    }
                    else if(ext == "cert") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/CERT.png");
                    }
                    else if(ext == "nca") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/NCA.png");
                    }
                    else if(ext == "nxtheme") {
                        fs_icon = g_Settings.PathForResource("/FileSystem/NXTheme.png");
                    }
                    else {
                        fs_icon = g_Settings.PathForResource("/FileSystem/File.png");
                    }
                }
                const auto option = this->CreateShowDialog(cfg::strings::Main.GetString(222), cfg::strings::Main.GetString(223) + "\n(" + g_Clipboard + ")", { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true, fs_icon);
                if(option == 0) {
                    const auto item_name = fs::GetFileName(g_Clipboard);
                    this->LoadLayout(this->GetCopyLayout());
                    this->GetCopyLayout()->StartCopy(g_Clipboard, this->partition_browser_lyt->GetExplorer()->FullPathFor(item_name));
                    g_MainApplication->LoadLayout(this->partition_browser_lyt);
                    this->partition_browser_lyt->UpdateElements();
                    g_Clipboard = "";
                }
            }
            else {
                this->ShowNotification(cfg::strings::Main.GetString(224));
            }
        }
        else if(down & HidNpadButton_L) {
            const auto file_name = AskForText(cfg::strings::Main.GetString(225), "");
            if(!file_name.empty()) {
                const auto full_path = this->partition_browser_lyt->GetExplorer()->FullPathFor(file_name);
                if(this->partition_browser_lyt->GetExplorer()->IsFile(full_path) || this->partition_browser_lyt->GetExplorer()->IsDirectory(full_path)) {
                    HandleResult(err::result::ResultEntryAlreadyPresent, cfg::strings::Main.GetString(255));
                }
                else {
                    this->partition_browser_lyt->GetExplorer()->CreateFile(full_path);
                    this->ShowNotification(cfg::strings::Main.GetString(227) + " \'" + file_name + "\'");
                    this->partition_browser_lyt->UpdateElements();
                }
            }
        }
        else if(down & HidNpadButton_R) {
            const auto dir_name = AskForText(cfg::strings::Main.GetString(250), "");
            if(!dir_name.empty()) {
                const auto full_path = this->partition_browser_lyt->GetExplorer()->FullPathFor(dir_name);
                if(this->partition_browser_lyt->GetExplorer()->IsFile(full_path) || this->partition_browser_lyt->GetExplorer()->IsDirectory(full_path)) {
                    HandleResult(err::result::ResultEntryAlreadyPresent, cfg::strings::Main.GetString(255));
                }
                else {
                    this->partition_browser_lyt->GetExplorer()->CreateDirectory(full_path);
                    this->ShowNotification(cfg::strings::Main.GetString(228) + " \'" + dir_name + "\'");
                    this->partition_browser_lyt->UpdateElements();
                }
            }
        }
    }

    void MainApplication::exploreMenu_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::pcExplore_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->UnloadMenuData();
            this->LoadMenuData(cfg::strings::Main.GetString(277), "Storage", cfg::strings::Main.GetString(278));
            this->LoadLayout(this->explore_menu_lyt);
        }
    }

    void MainApplication::fileContent_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->LoadLayout(this->partition_browser_lyt);
        }
        else if((down & HidNpadButton_Down) || (down & HidNpadButton_StickLDown) || (held & HidNpadButton_StickRDown)) {
            this->file_content_lyt->ScrollDown();
        }
        else if((down & HidNpadButton_Up) || (down & HidNpadButton_StickLUp) || (held & HidNpadButton_StickRUp)) {
            this->file_content_lyt->ScrollUp();
        }
    }

    void MainApplication::contentInformation_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->LoadMenuData(cfg::strings::Main.GetString(187), "Storage", cfg::strings::Main.GetString(189));
            this->LoadLayout(this->storage_cnts_lyt);
        }
    }

    void MainApplication::storageContents_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->LoadMenuData(cfg::strings::Main.GetString(187), "Storage", cfg::strings::Main.GetString(33));
            this->LoadLayout(this->cnt_manager_lyt);
        }
    }

    void MainApplication::contentManager_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::unusedTickets_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::account_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::amiibo_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::settings_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::ownSettings_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->UnloadMenuData();
            this->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
            this->LoadLayout(this->settings_lyt);
        }
    }

    void MainApplication::memory_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->UnloadMenuData();
            this->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
            this->LoadLayout(this->settings_lyt);
        }
    }

    void MainApplication::webBrowser_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::about_Input(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_B) {
            this->ReturnToMainMenu();
        }
    }

    void MainApplication::userImage_OnClick() {
        if(acc::SelectUser()) {
            acc::CacheSelectedUserIcon();
            this->ShowNotification(cfg::strings::Main.GetString(324));
        }
    }

    void MainApplication::helpImage_OnClick() {
        this->CreateShowDialog(cfg::strings::Main.GetString(162), cfg::strings::Main.GetString(342) + "\n\n" + cfg::strings::Main.GetString(343) + "\n" + cfg::strings::Main.GetString(344) + "\n" + cfg::strings::Main.GetString(345) + "\n" + cfg::strings::Main.GetString(346) + "\n" + cfg::strings::Main.GetString(347), {cfg::strings::Main.GetString(234)}, false);
    }

    void MainApplication::OnInput(const u64 down, const u64 up, const u64 held) {
        if(down & HidNpadButton_Plus) {
            this->CloseWithFadeOut();
        }
        else if(down & HidNpadButton_Minus) {
            this->helpImage_OnClick();
        }
        else if((down & HidNpadButton_ZL) || (down & HidNpadButton_ZR)) {
            ShowPowerTasksDialog(cfg::strings::Main.GetString(229), cfg::strings::Main.GetString(230));
        }
    }

    void UpdateClipboard(const std::string &path) {
        SetClipboard(path);
        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(g_MainApplication->GetBrowserLayout()->GetExplorer()->IsFile(path) ? 257 : 258));
    }

}