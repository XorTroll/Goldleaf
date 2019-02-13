
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Goldleaf>
#include <pu/Plutonium>

namespace gleaf::ui
{
    class MainMenuLayout : public pu::Layout
    {
        public:
            MainMenuLayout();
            void optionMenu_SelectionChanged();
            void sdcardMenuItem_Click();
            void nandMenuItem_Click();
            void usbMenuItem_Click();
            void titleMenuItem_Click();
            void ticketMenuItem_Click();
            void webMenuItem_Click();
            void accountMenuItem_Click();
            void sysinfoMenuItem_Click();
            void aboutMenuItem_Click();
        private:
            pu::element::Menu *optionMenu;
            pu::element::MenuItem *sdcardMenuItem;
            pu::element::MenuItem *nandMenuItem;
            pu::element::MenuItem *usbMenuItem;
            pu::element::MenuItem *titleMenuItem;
            pu::element::MenuItem *ticketMenuItem;
            pu::element::MenuItem *webMenuItem;
            pu::element::MenuItem *accountMenuItem;
            pu::element::MenuItem *sysinfoMenuItem;
            pu::element::MenuItem *aboutMenuItem;
    };

    class PartitionBrowserLayout : public pu::Layout
    {
        public:
            PartitionBrowserLayout(fs::Partition Partition);
            void ChangePartition(fs::Partition Partition);
            void UpdateElements();
            bool GoBack();
            bool WarnNANDWriteAccess();
            void fsItems_Click();
            void fsItems_Click_Y();
            fs::Explorer *GetExplorer();
        private:
            fs::Explorer *gexp;
            std::vector<std::string> elems;
            pu::element::Menu *browseMenu;
            pu::element::TextBlock *dirEmptyText;
    };

    class FileContentLayout : public pu::Layout
    {
        public:
            FileContentLayout();
            void LoadFile(std::string Path, pu::Layout *Prev, bool Hex);
            void Update();
            void ScrollUp();
            void ScrollDown();
            pu::Layout *GetPreviousLayout();
        private:
            u32 loffset;
            u32 rlines;
            bool mode;
            std::string pth;
            pu::element::TextBlock *cntText;
            pu::Layout *prev;
    };

    class CopyLayout : public pu::Layout
    {
        public:
            CopyLayout();
            void StartCopy(std::string Path, std::string NewPath, bool Directory, pu::Layout *Prev);
        private:
            pu::element::TextBlock *infoText;
            pu::element::ProgressBar *copyBar;
    };

    class InstallLayout : public pu::Layout
    {
        public:
            InstallLayout();
            void StartInstall(nsp::Installer *Inst, pu::Layout *Prev, bool Delete, std::string Input, std::string PInput);
            void LogError(InstallerResult Res);
        private:
            pu::element::TextBlock *installText;
            pu::element::ProgressBar *installBar;
    };

    class USBInstallLayout : public pu::Layout
    {
        public:
            USBInstallLayout();
            void StartUSBConnection();
            void LogError(InstallerResult Res);
        private:
            pu::element::TextBlock *installText;
            pu::element::ProgressBar *installBar;
    };

    class ContentInformationLayout : public pu::Layout
    {
        public:
            ContentInformationLayout();
            void UpdateElements();
            void contentInfo_Click();
            void contents_Click();
            void LoadContent(horizon::Title Content);
        private:
            horizon::Title content;
            horizon::TitleContents contents;
            pu::element::MenuItem *contentInfo;
            pu::element::Menu *optionsMenu;
    };

    class StorageContentsLayout : public pu::Layout
    {
        public:
            StorageContentsLayout();
            void contents_Click();
            void LoadFromStorage(Storage Location);
        private:
            std::vector<horizon::Title> contents;
            pu::element::TextBlock *noContentsText;
            pu::element::Menu *contentsMenu;
    };

    class ContentManagerLayout : public pu::Layout
    {
        public:
            ContentManagerLayout();
            void sdCardMenuItem_Click();
            void nandUserMenuItem_Click();
            void nandSystemMenuItem_Click();
            void gameCartMenuItem_Click();
        private:
            pu::element::MenuItem *sdCardMenuItem;
            pu::element::MenuItem *nandUserMenuItem;
            pu::element::MenuItem *nandSystemMenuItem;
            pu::element::MenuItem *gameCartMenuItem;
            pu::element::Menu *typesMenu;
    };

    class TitleDumperLayout : public pu::Layout
    {
        public:
            TitleDumperLayout();
            void StartDump(horizon::Title &Target);
        private:
            pu::element::TextBlock *dumpText;
            pu::element::ProgressBar *ncaBar;
    };

    class TicketManagerLayout : public pu::Layout
    {
        public:
            TicketManagerLayout();
            void UpdateElements();
            void tickets_Click();
        private:
            std::vector<horizon::Ticket> tickets;
            pu::element::TextBlock *notTicketsText;
            pu::element::Menu *ticketsMenu;
    };

    class AccountLayout : public pu::Layout
    {
        public:
            AccountLayout();
            void Load(u128 UserId);
            void CleanData();
            void optsRename_Click();
            void optsIcon_Click();
            void optsDelete_Click();
        private:
            pu::element::Menu *optsMenu;
            u128 uid;
            AccountProfile prf;
            AccountProfileBase *pbase;
            AccountUserData *udata;
            acc::ProfileEditor *pred;
    };

    class SystemInfoLayout : public pu::Layout
    {
        public:
            SystemInfoLayout();
            void UpdateElements();
        private:
            pu::element::TextBlock *fwText;
            pu::element::TextBlock *sdText;
            pu::element::ProgressBar *sdBar;
            pu::element::TextBlock *sdFreeText;
            pu::element::TextBlock *nandText;
            pu::element::ProgressBar *nandBar;
            pu::element::TextBlock *nandFreeText;
            pu::element::TextBlock *safeText;
            pu::element::ProgressBar *safeBar;
            pu::element::TextBlock *systemText;
            pu::element::ProgressBar *systemBar;
            pu::element::TextBlock *userText;
            pu::element::ProgressBar *userBar;
    };

    class AboutLayout : public pu::Layout
    {
        public:
            AboutLayout();
        private:
            pu::element::Image *logoImage;
    };

    class MainApplication : public pu::Application
    {
        public:
            MainApplication();
            void UpdateFooter(std::string Text);
            void UpdateValues();
            void LoadMenuData(std::string Name, std::string ImageName, std::string TempHead, bool CommonIcon = true);
            void LoadMenuHead(std::string Head);
            void UnloadMenuData();
            void sdBrowser_Input(u64 Down, u64 Up, u64 Held);
            void nandBrowser_Input(u64 Down, u64 Up, u64 Held);
            void fileContent_Input(u64 Down, u64 Up, u64 Held);
            void usbInstall_Input(u64 Down, u64 Up, u64 Held);
            void contentInformation_Input(u64 Down, u64 Up, u64 Held);
            void storageContents_Input(u64 Down, u64 Up, u64 Held);
            void contentManager_Input(u64 Down, u64 Up, u64 Held);
            void ticketManager_Input(u64 Down, u64 Up, u64 Held);
            void account_Input(u64 Down, u64 Up, u64 Held);
            void sysInfo_Input(u64 Down, u64 Up, u64 Held);
            void about_Input(u64 Down, u64 Up, u64 Held);
            void OnInput(u64 Down, u64 Up, u64 Held);
            MainMenuLayout *GetMainMenuLayout();
            PartitionBrowserLayout *GetSDBrowserLayout();
            PartitionBrowserLayout *GetNANDBrowserLayout();
            FileContentLayout *GetFileContentLayout();
            CopyLayout *GetCopyLayout();
            InstallLayout *GetInstallLayout();
            USBInstallLayout *GetUSBInstallLayout();
            ContentInformationLayout *GetContentInformationLayout();
            StorageContentsLayout *GetStorageContentsLayout();
            ContentManagerLayout *GetContentManagerLayout();
            TitleDumperLayout *GetTitleDumperLayout();
            TicketManagerLayout *GetTicketManagerLayout();
            AccountLayout *GetAccountLayout();
            SystemInfoLayout *GetSystemInfoLayout();
            AboutLayout *GetAboutLayout();
        private:
            u32 preblv;
            bool preisch;
            bool hasusb;
            std::string pretime;
            bool vfirst;
            MainMenuLayout *mainMenu;
            PartitionBrowserLayout *sdBrowser;
            PartitionBrowserLayout *nandBrowser;
            FileContentLayout *fileContent;
            CopyLayout *copy;
            InstallLayout *nspInstall;
            USBInstallLayout *usbInstall;
            ContentInformationLayout *contentInformation;
            StorageContentsLayout *storageContents;
            ContentManagerLayout *contentManager;
            TitleDumperLayout *titleDump;
            TicketManagerLayout *ticketManager;
            AccountLayout *account;
            SystemInfoLayout *sysInfo;
            AboutLayout *about;
            pu::element::Image *baseImage;
            pu::element::TextBlock *timeText;
            pu::element::TextBlock *batteryText;
            pu::element::Image *batteryImage;
            pu::element::Image *batteryChargeImage;
            pu::element::Image *menuBanner;
            pu::element::Image *menuImage;
            pu::element::Image *usbImage;
            pu::element::TextBlock *menuNameText;
            pu::element::TextBlock *menuHeadText;
            pu::element::TextBlock *footerText;
    };

    static const ColorScheme DefaultLight = { { 235, 235, 235, 255 }, { 220, 220, 220, 255 }, { 140, 140, 140, 255 }, { 15, 15, 15, 255 } };
    static const ColorScheme DefaultDark = { { 45, 45, 45, 255 }, { 70, 70, 70, 255 }, { 110, 110, 110, 255 }, { 225, 225, 225, 255 } };

    void UpdateClipboard(std::string Path);
    void ShowRebootShutDownDialog(std::string Title, std::string Message);
    std::string AskForText(std::string Guide, std::string Initial);
    u128 AskForUser();
    void SetMainApplication(MainApplication *MainApp);
}