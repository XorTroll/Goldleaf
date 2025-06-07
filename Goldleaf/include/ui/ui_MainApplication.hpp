
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

#pragma once
#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_AccountLayout.hpp>
#include <ui/ui_AmiiboDumpLayout.hpp>
#include <ui/ui_ClickableImage.hpp>
#include <ui/ui_ApplicationListLayout.hpp>
#include <ui/ui_ApplicationContentsLayout.hpp>
#include <ui/ui_ContentExportLayout.hpp>
#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_ExploreMenuLayout.hpp>
#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_InstallLayout.hpp>
#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MemoryLayout.hpp>
#include <ui/ui_OwnSettingsLayout.hpp>
#include <ui/ui_RemotePcExploreLayout.hpp>
#include <ui/ui_BrowserLayout.hpp>
#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_TicketsLayout.hpp>
#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_UpdateInstallLayout.hpp>
#include <ui/ui_WebBrowserLayout.hpp>

namespace ui {

    class MainApplication : public pu::ui::Application {
        private:
            struct MenuData {
                std::string name;
                pu::sdl2::TextureHandle::Ref img;
                std::string head;
            };

            u32 cur_battery_val;
            bool cur_is_charging;
            bool usb_ok;
            u32 cur_conn_strength;
            AccountUid cur_selected_user;
            std::string cur_time;
            bool read_values_once;
            MainMenuLayout::Ref main_menu_lyt;
            BrowserLayout::Ref browser_lyt;
            FileContentLayout::Ref file_content_lyt;
            CopyLayout::Ref copy_lyt;
            ExploreMenuLayout::Ref explore_menu_lyt;
            InstallLayout::Ref install_lyt;
            ApplicationListLayout::Ref app_list_lyt;
            ApplicationContentsLayout::Ref app_cnts_lyt;
            ContentExportLayout::Ref cnt_expt_lyt;
            TicketsLayout::Ref tiks_lyt;
            AccountLayout::Ref account_lyt;
            AmiiboDumpLayout::Ref amiibo_dump_lyt;
            SettingsLayout::Ref settings_lyt;
            OwnSettingsLayout::Ref own_settings_lyt;
            RemotePcExploreLayout::Ref remote_pc_explore_lyt;
            MemoryLayout::Ref memory_lyt;
            UpdateLayout::Ref update_lyt;
            UpdateInstallLayout::Ref update_install_lyt;
            WebBrowserLayout::Ref web_browser_lyt;
            AboutLayout::Ref about_lyt;
            pu::ui::elm::Image::Ref base_img;
            pu::ui::elm::TextBlock::Ref time_text;
            pu::ui::elm::TextBlock::Ref battery_text;
            pu::ui::elm::Image::Ref battery_img;
            pu::ui::elm::Image::Ref battery_charge_img;
            pu::ui::elm::Image::Ref menu_banner_img;
            pu::ui::elm::TextBlock::Ref menu_version_text;
            pu::ui::elm::Image::Ref menu_img;
            pu::ui::elm::Image::Ref usb_img;
            pu::ui::elm::Image::Ref conn_img;
            pu::ui::elm::TextBlock::Ref ip_text;
            pu::ui::elm::TextBlock::Ref menu_name_text;
            pu::ui::elm::TextBlock::Ref menu_head_text;
            ClickableImage::Ref user_img;
            ClickableImage::Ref help_img;
            pu::ui::extras::Toast::Ref toast;
            std::chrono::time_point<std::chrono::steady_clock> start_time;
            std::stack<MenuData> menu_data_stack;
            std::stack<pu::ui::Layout::Ref> lyt_stack;

            void DoLoadMenuData(const std::string &name, pu::sdl2::TextureHandle::Ref img, const std::string &temp_head);
            void helpImage_OnClick();
        public:
            using Application::Application;
            PU_SMART_CTOR(MainApplication)

            void OnLoad() override;

            void ShowNotification(const std::string &text);
            int DisplayDialog(const std::string &title, const std::string &content, const std::vector<std::string> &opts, const bool use_last_opt_as_cancel, pu::sdl2::TextureHandle::Ref icon = {});

            void UpdateValues();
            void ReturnToParentLayout();

            void LoadMenuData(const bool push_new, const std::string &name, pu::sdl2::TextureHandle::Ref icon, const std::string &temp_head);

            inline void LoadFileMenuData(const bool push_new, const std::string &name, const std::string &img_path, const std::string &temp_head) {
                this->LoadMenuData(push_new, name, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(img_path)), temp_head);
            }

            inline void LoadCommonIconMenuData(const bool push_new, const std::string &name, const CommonIconKind kind, const std::string &temp_head) {
                this->LoadMenuData(push_new, name, GetCommonIcon(kind), temp_head);
            }

            void PopMenuData();
            void LoadMenuHead(const std::string &head);
            void UnloadMenuData();

            void ShowLayout(pu::ui::Layout::Ref lyt);
            void PopLayout();
            void ClearLayout(pu::ui::Layout::Ref lyt);

            void PickUser();

            void OnInput(const u64 down, const u64 up, const u64 held);
            
            inline MainMenuLayout::Ref &GetMainMenuLayout() {
                return this->main_menu_lyt;
            }

            inline BrowserLayout::Ref &GetBrowserLayout() {
                return this->browser_lyt;
            }

            inline FileContentLayout::Ref &GetFileContentLayout() {
                return this->file_content_lyt;
            }

            inline CopyLayout::Ref &GetCopyLayout() {
                return this->copy_lyt;
            }

            inline ExploreMenuLayout::Ref &GetExploreMenuLayout() {
                return this->explore_menu_lyt;
            }

            inline InstallLayout::Ref &GetInstallLayout() {
                return this->install_lyt;
            }

            inline ApplicationListLayout::Ref &GetApplicationListLayout() {
                return this->app_list_lyt;
            }

            inline ApplicationContentsLayout::Ref &GetApplicationContentsLayout() {
                return this->app_cnts_lyt;
            }

            inline ContentExportLayout::Ref &GetContentExportLayout() {
                return this->cnt_expt_lyt;
            }

            inline TicketsLayout::Ref &GetTicketsLayout() {
                return this->tiks_lyt;
            }

            inline AccountLayout::Ref &GetAccountLayout() {
                return this->account_lyt;
            }

            inline AmiiboDumpLayout::Ref &GetAmiiboDumpLayout() {
                return this->amiibo_dump_lyt;
            }

            inline SettingsLayout::Ref &GetSettingsLayout() {
                return this->settings_lyt;
            }

            inline OwnSettingsLayout::Ref &GetOwnSettingsLayout() {
                return this->own_settings_lyt;
            }

            inline RemotePcExploreLayout::Ref &GetRemotePcExploreLayout() {
                return this->remote_pc_explore_lyt;
            }

            inline MemoryLayout::Ref &GetMemoryLayout() {
                return this->memory_lyt;
            }

            inline UpdateLayout::Ref &GetUpdateLayout() {
                return this->update_lyt;
            }

            inline UpdateInstallLayout::Ref &GetUpdateInstallLayout() {
                return this->update_install_lyt;
            }

            inline WebBrowserLayout::Ref &GetWebBrowserLayout() {
                return this->web_browser_lyt;
            }

            inline AboutLayout::Ref &GetAboutLayout() {
                return this->about_lyt;
            }
    };

    void UpdateClipboard(const std::string &path);

}
