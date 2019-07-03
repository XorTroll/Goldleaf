
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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
#include <ui/ui_PartitionBrowserLayout.hpp>
#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_StorageContentsLayout.hpp>
#include <ui/ui_SystemInfoLayout.hpp>
#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_TitleDumperLayout.hpp>
#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_USBDrivesLayout.hpp>

namespace ui
{
    class MainApplication : public pu::ui::Application
    {
        public:
            MainApplication();
            ~MainApplication();
            void ShowNotification(std::string Text);
            void UpdateValues();
            void LoadMenuData(std::string Name, std::string ImageName, std::string TempHead, bool CommonIcon = true);
            void LoadMenuHead(std::string Head);
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
            void sysInfo_Input(u64 Down, u64 Up, u64 Held);
            void about_Input(u64 Down, u64 Up, u64 Held);
            void OnInput(u64 Down, u64 Up, u64 Held);
            MainMenuLayout *GetMainMenuLayout();
            PartitionBrowserLayout *GetBrowserLayout();
            FileContentLayout *GetFileContentLayout();
            CopyLayout *GetCopyLayout();
            ExploreMenuLayout *GetExploreMenuLayout();
            PCExploreLayout *GetPCExploreLayout();
            USBDrivesLayout *GetUSBDrivesLayout();
            InstallLayout *GetInstallLayout();
            ContentInformationLayout *GetContentInformationLayout();
            StorageContentsLayout *GetStorageContentsLayout();
            ContentManagerLayout *GetContentManagerLayout();
            TitleDumperLayout *GetTitleDumperLayout();
            UnusedTicketsLayout *GetUnusedTicketsLayout();
            AccountLayout *GetAccountLayout();
            AmiiboDumpLayout *GetAmiiboDumpLayout();
            SystemInfoLayout *GetSystemInfoLayout();
            UpdateLayout *GetUpdateLayout();
            AboutLayout *GetAboutLayout();
        private:
            u32 preblv;
            bool preisch;
            bool hasusb;
            u32 connstate;
            std::string pretime;
            bool vfirst;
            MainMenuLayout *mainMenu;
            PartitionBrowserLayout *browser;
            FileContentLayout *fileContent;
            CopyLayout *copy;
            ExploreMenuLayout *exploreMenu;
            PCExploreLayout *pcExplore;
            USBDrivesLayout *usbDrives;
            InstallLayout *nspInstall;
            ContentInformationLayout *contentInformation;
            StorageContentsLayout *storageContents;
            ContentManagerLayout *contentManager;
            TitleDumperLayout *titleDump;
            UnusedTicketsLayout *unusedTickets;
            AccountLayout *account;
            AmiiboDumpLayout *amiibo;
            SystemInfoLayout *sysInfo;
            UpdateLayout* update;
            AboutLayout *about;
            pu::ui::elm::Image *baseImage;
            pu::ui::elm::TextBlock *timeText;
            pu::ui::elm::TextBlock *batteryText;
            pu::ui::elm::Image *batteryImage;
            pu::ui::elm::Image *batteryChargeImage;
            pu::ui::elm::Image *menuBanner;
            pu::ui::elm::Image *menuImage;
            pu::ui::elm::Image *usbImage;
            pu::ui::elm::Image *connImage;
            pu::ui::elm::TextBlock *ipText;
            pu::ui::elm::TextBlock *menuNameText;
            pu::ui::elm::TextBlock *menuHeadText;
            pu::ui::extras::Toast *toast;
            bool updshown;
            std::chrono::time_point<std::chrono::steady_clock> start;
    }; // { 50, 138, 188, 255 }

    static const ColorScheme DefaultLight = { { 124, 199, 239, 255 }, { 196, 224, 239, 255 }, { 168, 214, 237, 255 }, { 15, 15, 15, 255 } };
    static const ColorScheme DefaultDark = { { 29, 81, 114, 255 }, { 37, 104, 145, 255 }, { 45, 124, 173, 255 }, { 225, 225, 225, 255 } };

    void UpdateClipboard(std::string Path);
}