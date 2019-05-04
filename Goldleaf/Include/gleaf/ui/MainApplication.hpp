
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/ui/AboutLayout.hpp>
#include <gleaf/ui/AccountLayout.hpp>
#include <gleaf/ui/ContentInformationLayout.hpp>
#include <gleaf/ui/ContentManagerLayout.hpp>
#include <gleaf/ui/CopyLayout.hpp>
#include <gleaf/ui/ExploreMenuLayout.hpp>
#include <gleaf/ui/FileContentLayout.hpp>
#include <gleaf/ui/InstallLayout.hpp>
#include <gleaf/ui/MainMenuLayout.hpp>
#include <gleaf/ui/PartitionBrowserLayout.hpp>
#include <gleaf/ui/PCExploreLayout.hpp>
#include <gleaf/ui/StorageContentsLayout.hpp>
#include <gleaf/ui/SystemInfoLayout.hpp>
#include <gleaf/ui/TicketManagerLayout.hpp>
#include <gleaf/ui/TitleDumperLayout.hpp>
#include <gleaf/ui/UpdateLayout.hpp>
#include <gleaf/ui/USBDrivesLayout.hpp>

namespace gleaf::ui
{
    class MainApplication : public pu::Application
    {
        public:
            MainApplication(StartMode Mode);
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
            void ticketManager_Input(u64 Down, u64 Up, u64 Held);
            void account_Input(u64 Down, u64 Up, u64 Held);
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
            TicketManagerLayout *GetTicketManagerLayout();
            AccountLayout *GetAccountLayout();
            SystemInfoLayout *GetSystemInfoLayout();
            UpdateLayout *GetUpdateLayout();
            AboutLayout *GetAboutLayout();
        private:
            StartMode stmode;
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
            TicketManagerLayout *ticketManager;
            AccountLayout *account;
            SystemInfoLayout *sysInfo;
            UpdateLayout* update;
            AboutLayout *about;
            pu::element::Image *baseImage;
            pu::element::TextBlock *timeText;
            pu::element::TextBlock *batteryText;
            pu::element::Image *batteryImage;
            pu::element::Image *batteryChargeImage;
            pu::element::Image *menuBanner;
            pu::element::Image *menuImage;
            pu::element::Image *usbImage;
            pu::element::Image *connImage;
            pu::element::TextBlock *ipText;
            pu::element::TextBlock *menuNameText;
            pu::element::TextBlock *menuHeadText;
            pu::overlay::Toast *toast;
            bool updshown;
            std::chrono::time_point<std::chrono::steady_clock> start;
    };

    static const ColorScheme DefaultLight = { { 235, 235, 235, 255 }, { 220, 220, 220, 255 }, { 140, 140, 140, 255 }, { 15, 15, 15, 255 } };
    static const ColorScheme DefaultDark = { { 45, 45, 45, 255 }, { 70, 70, 70, 255 }, { 110, 110, 110, 255 }, { 225, 225, 225, 255 } };

    void UpdateClipboard(std::string Path);
    void SetMainApplication(MainApplication *MainApp);
}