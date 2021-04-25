
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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
#include <ui/ui_ContentInformationLayout.hpp>
#include <ui/ui_ContentManagerLayout.hpp>
#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_ExploreMenuLayout.hpp>
#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_InstallLayout.hpp>
#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MemoryLayout.hpp>
#include <ui/ui_PartitionBrowserLayout.hpp>
#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_StorageContentsLayout.hpp>
#include <ui/ui_TitleDumperLayout.hpp>
#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_UpdateInstallLayout.hpp>
#include <ui/ui_WebBrowserLayout.hpp>

namespace ui {

    class MainApplication : public pu::ui::Application {
        private:
            u32 cur_battery_val;
            bool cur_is_charging;
            bool usb_ok;
            u32 cur_conn_strength;
            AccountUid cur_selected_user;
            String cur_time;
            bool read_values_once;
            MainMenuLayout::Ref main_menu_lyt;
            PartitionBrowserLayout::Ref partition_browser_lyt;
            FileContentLayout::Ref file_content_lyt;
            CopyLayout::Ref copy_lyt;
            ExploreMenuLayout::Ref explore_menu_lyt;
            PCExploreLayout::Ref pc_explore_lyt;
            InstallLayout::Ref install_lyt;
            ContentInformationLayout::Ref cnt_information_lyt;
            StorageContentsLayout::Ref storage_cnts_lyt;
            ContentManagerLayout::Ref cnt_manager_lyt;
            TitleDumperLayout::Ref title_dumper_lyt;
            UnusedTicketsLayout::Ref unused_tiks_lyt;
            AccountLayout::Ref account_lyt;
            AmiiboDumpLayout::Ref amiibo_dump_lyt;
            SettingsLayout::Ref settings_lyt;
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
        public:
            using Application::Application;
            PU_SMART_CTOR(MainApplication)

            void OnLoad() override;

            void ShowNotification(String text);
            void UpdateValues();
            void ReturnToMainMenu();
            void LoadMenuData(String name, const std::string &img_name, String temp_head, bool is_common_icon = true);
            void LoadMenuHead(String head);
            void UnloadMenuData();
            void browser_Input(u64 down, u64 up, u64 held);
            void exploreMenu_Input(u64 down, u64 up, u64 held);
            void pcExplore_Input(u64 down, u64 up, u64 held);
            void fileContent_Input(u64 down, u64 up, u64 held);
            void contentInformation_Input(u64 down, u64 up, u64 held);
            void storageContents_Input(u64 down, u64 up, u64 held);
            void contentManager_Input(u64 down, u64 up, u64 held);
            void unusedTickets_Input(u64 down, u64 up, u64 held);
            void account_Input(u64 down, u64 up, u64 held);
            void amiibo_Input(u64 down, u64 up, u64 held);
            void settings_Input(u64 down, u64 up, u64 held);
            void memory_Input(u64 down, u64 up, u64 held);
            void webBrowser_Input(u64 down, u64 up, u64 held);
            void about_Input(u64 down, u64 up, u64 held);
            void userImage_OnClick();
            void helpImage_OnClick();
            void ReloadUser(AccountUid User);
            void OnInput(u64 down, u64 up, u64 held);
            
            inline MainMenuLayout::Ref &GetMainMenuLayout() {
                return this->main_menu_lyt;
            }

            inline PartitionBrowserLayout::Ref &GetBrowserLayout() {
                return this->partition_browser_lyt;
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

            inline PCExploreLayout::Ref &GetPCExploreLayout() {
                return this->pc_explore_lyt;
            }

            inline InstallLayout::Ref &GetInstallLayout() {
                return this->install_lyt;
            }

            inline ContentInformationLayout::Ref &GetContentInformationLayout() {
                return this->cnt_information_lyt;
            }

            inline StorageContentsLayout::Ref &GetStorageContentsLayout() {
                return this->storage_cnts_lyt;
            }

            inline ContentManagerLayout::Ref &GetContentManagerLayout() {
                return this->cnt_manager_lyt;
            }

            inline TitleDumperLayout::Ref &GetTitleDumperLayout() {
                return this->title_dumper_lyt;
            }

            inline UnusedTicketsLayout::Ref &GetUnusedTicketsLayout() {
                return this->unused_tiks_lyt;
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

    constexpr ColorScheme DefaultLight = { { 124, 199, 239, 255 }, { 196, 224, 239, 255 }, { 168, 214, 237, 255 }, { 15, 15, 15, 255 } };
    constexpr ColorScheme DefaultDark = { { 29, 81, 114, 255 }, { 37, 104, 145, 255 }, { 45, 124, 173, 255 }, { 225, 225, 225, 255 } };

    void UpdateClipboard(String Path);

    void ApplyRandomScheme();

}