
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

#pragma once
#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_AccountLayout.hpp>
#include <ui/ui_AmiiboDumpLayout.hpp>
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
#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_TitleDumperLayout.hpp>
#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_USBDrivesLayout.hpp>

#include <ui/ui_ClickableImage.hpp>

namespace ui
{
    class MainApplication : public pu::ui::Application
    {
        public:
            MainApplication();
            PU_SMART_CTOR(MainApplication)

            void ShowNotification(pu::String Text);
            void UpdateValues();
            void LoadMenuData(pu::String Name, std::string ImageName, pu::String TempHead, bool CommonIcon = true);
            void LoadMenuHead(pu::String Head);
            void UnloadMenuData();
            void browser_Input(u64 Down, u64 Up, u64 Held);
            void exploreMenu_Input(u64 Down, u64 Up, u64 Held);
            void pcExplore_Input(u64 Down, u64 Up, u64 Held);
            void usbDrives_Input(u64 Down, u64 Up, u64 Held);
            void fileContent_Input(u64 Down, u64 Up, u64 Held);
            void contentInformation_Input(u64 Down, u64 Up, u64 Held);
            void storageContents_Input(u64 Down, u64 Up, u64 Held);
            void contentManager_Input(u64 Down, u64 Up, u64 Held);
            void unusedTickets_Input(u64 Down, u64 Up, u64 Held);
            void account_Input(u64 Down, u64 Up, u64 Held);
            void amiibo_Input(u64 Down, u64 Up, u64 Held);
            void settings_Input(u64 Down, u64 Up, u64 Held);
            void memory_Input(u64 Down, u64 Up, u64 Held);
            void about_Input(u64 Down, u64 Up, u64 Held);
            void userImage_OnClick();
            void helpImage_OnClick();
            void ReloadUser(u128 User);
            void OnInput(u64 Down, u64 Up, u64 Held);
            MainMenuLayout::Ref &GetMainMenuLayout();
            PartitionBrowserLayout::Ref &GetBrowserLayout();
            FileContentLayout::Ref &GetFileContentLayout();
            CopyLayout::Ref &GetCopyLayout();
            ExploreMenuLayout::Ref &GetExploreMenuLayout();
            PCExploreLayout::Ref &GetPCExploreLayout();
            USBDrivesLayout::Ref &GetUSBDrivesLayout();
            InstallLayout::Ref &GetInstallLayout();
            ContentInformationLayout::Ref &GetContentInformationLayout();
            StorageContentsLayout::Ref &GetStorageContentsLayout();
            ContentManagerLayout::Ref &GetContentManagerLayout();
            TitleDumperLayout::Ref &GetTitleDumperLayout();
            UnusedTicketsLayout::Ref &GetUnusedTicketsLayout();
            AccountLayout::Ref &GetAccountLayout();
            AmiiboDumpLayout::Ref &GetAmiiboDumpLayout();
            SettingsLayout::Ref &GetSettingsLayout();
            MemoryLayout::Ref &GetMemoryLayout();
            UpdateLayout::Ref &GetUpdateLayout();
            AboutLayout::Ref &GetAboutLayout();
        private:
            u32 preblv;
            bool preisch;
            bool hasusb;
            u32 connstate;
            u128 seluser;
            pu::String pretime;
            bool vfirst;
            MainMenuLayout::Ref mainMenu;
            PartitionBrowserLayout::Ref browser;
            FileContentLayout::Ref fileContent;
            CopyLayout::Ref copy;
            ExploreMenuLayout::Ref exploreMenu;
            PCExploreLayout::Ref pcExplore;
            USBDrivesLayout::Ref usbDrives;
            InstallLayout::Ref nspInstall;
            ContentInformationLayout::Ref contentInformation;
            StorageContentsLayout::Ref storageContents;
            ContentManagerLayout::Ref contentManager;
            TitleDumperLayout::Ref titleDump;
            UnusedTicketsLayout::Ref unusedTickets;
            AccountLayout::Ref account;
            AmiiboDumpLayout::Ref amiibo;
            SettingsLayout::Ref settings;
            MemoryLayout::Ref memory;
            UpdateLayout::Ref update;
            AboutLayout::Ref about;
            pu::ui::elm::Image::Ref baseImage;
            pu::ui::elm::TextBlock::Ref timeText;
            pu::ui::elm::TextBlock::Ref batteryText;
            pu::ui::elm::Image::Ref batteryImage;
            pu::ui::elm::Image::Ref batteryChargeImage;
            pu::ui::elm::Image::Ref menuBanner;
            pu::ui::elm::Image::Ref menuImage;
            pu::ui::elm::Image::Ref usbImage;
            pu::ui::elm::Image::Ref connImage;
            pu::ui::elm::TextBlock::Ref ipText;
            pu::ui::elm::TextBlock::Ref menuNameText;
            pu::ui::elm::TextBlock::Ref menuHeadText;
            ClickableImage::Ref userImage;
            ClickableImage::Ref helpImage;
            pu::ui::extras::Toast::Ref toast;
            bool welcomeshown;
            std::chrono::time_point<std::chrono::steady_clock> start;
    };

    static const ColorScheme DefaultLight = { { 124, 199, 239, 255 }, { 196, 224, 239, 255 }, { 168, 214, 237, 255 }, { 15, 15, 15, 255 } };
    static const ColorScheme DefaultDark = { { 29, 81, 114, 255 }, { 37, 104, 145, 255 }, { 45, 124, 173, 255 }, { 225, 225, 225, 255 } };

    void UpdateClipboard(pu::String Path);
}