
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
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
            void cfwConfigMenuItem_Click();
            void sysinfoMenuItem_Click();
            void aboutMenuItem_Click();
        private:
            pu::element::Menu *optionMenu;
            pu::element::MenuItem *sdcardMenuItem;
            pu::element::MenuItem *nandMenuItem;
            pu::element::MenuItem *usbMenuItem;
            pu::element::MenuItem *titleMenuItem;
            pu::element::MenuItem *ticketMenuItem;
            pu::element::MenuItem *cfwConfigMenuItem;
            pu::element::MenuItem *sysinfoMenuItem;
            pu::element::MenuItem *aboutMenuItem;
    };

    class PartitionBrowserLayout : public pu::Layout
    {
        public:
            PartitionBrowserLayout(gleaf::fs::Partition Partition);
            void ChangePartition(gleaf::fs::Partition Partition);
            void UpdateElements();
            bool GoBack();
            bool WarnNANDWriteAccess();
            void fsItems_Click();
            void fsItems_Click_Y();
            gleaf::fs::Explorer *GetExplorer();
        private:
            gleaf::fs::Explorer *gexp;
            std::vector<std::string> elems;
            pu::element::Menu *browseMenu;
            pu::element::TextBlock *dirEmptyText;
    };

    class InstallLayout : public pu::Layout
    {
        public:
            InstallLayout();
            void StartInstall(gleaf::nsp::Installer *Inst, pu::Layout *Prev, bool Delete, std::string Input = "");
            void LogError(gleaf::InstallerResult Res);
        private:
            pu::element::TextBlock *installText;
            pu::element::ProgressBar *installBar;
    };

    class USBInstallLayout : public pu::Layout
    {
        public:
            USBInstallLayout();
            void StartUSBConnection();
            void LogError(gleaf::InstallerResult Res);
        private:
            pu::element::TextBlock *installText;
            pu::element::ProgressBar *installBar;
    };

    class ThemeInstallLayout : public pu::Layout
    {
        public:
            ThemeInstallLayout();
            void StartInstall(gleaf::theme::ThemeFileManifest &NXTheme, gleaf::sarc::SARC::SarcData &SData, std::string CFWPath);
        private:
            pu::element::TextBlock *infoText;
    };

    class TitleManagerLayout : public pu::Layout
    {
        public:
            TitleManagerLayout();
            void UpdateElements();
            void titles_Click();
            std::vector<gleaf::horizon::Title> GetTitles();
        private:
            std::vector<gleaf::horizon::Title> titles;
            pu::element::TextBlock *notTitlesText;
            pu::element::Menu *titlesMenu;
    };

    class TicketManagerLayout : public pu::Layout
    {
        public:
            TicketManagerLayout();
            void UpdateElements();
            void tickets_Click();
        private:
            std::vector<gleaf::horizon::Ticket> tickets;
            pu::element::TextBlock *notTicketsText;
            pu::element::Menu *ticketsMenu;
    };

    class CFWConfigLayout : public pu::Layout
    {
        public:
            CFWConfigLayout();
            void UpdateElements();
            void cfws_Click();
        private:
            pu::element::Menu *cfwsMenu;
            std::vector<std::string> cfws;
            std::vector<std::string> cfwnms;
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
            pu::element::TextBlock *modeText;
    };

    class MainApplication : public pu::Application
    {
        public:
            MainApplication();
            void UpdateFooter(std::string Text);
            void UpdateValues();
            void sdBrowser_Input(u64 Down, u64 Up, u64 Held);
            void nandBrowser_Input(u64 Down, u64 Up, u64 Held);
            void usbInstall_Input(u64 Down, u64 Up, u64 Held);
            void titleManager_Input(u64 Down, u64 Up, u64 Held);
            void ticketManager_Input(u64 Down, u64 Up, u64 Held);
            void cfwConfig_Input(u64 Down, u64 Up, u64 Held);
            void sysInfo_Input(u64 Down, u64 Up, u64 Held);
            void about_Input(u64 Down, u64 Up, u64 Held);
            void OnInput(u64 Down, u64 Up, u64 Held);
            MainMenuLayout *GetMainMenuLayout();
            PartitionBrowserLayout *GetSDBrowserLayout();
            PartitionBrowserLayout *GetNANDBrowserLayout();
            InstallLayout *GetInstallLayout();
            USBInstallLayout *GetUSBInstallLayout();
            ThemeInstallLayout *GetThemeInstallLayout();
            TitleManagerLayout *GetTitleManagerLayout();
            TicketManagerLayout *GetTicketManagerLayout();
            CFWConfigLayout *GetCFWConfigLayout();
            SystemInfoLayout *GetSystemInfoLayout();
            AboutLayout *GetAboutLayout();
        private:
            u32 preblv;
            bool preisch;
            std::string pretime;
            bool vfirst;
            MainMenuLayout *mainMenu;
            PartitionBrowserLayout *sdBrowser;
            PartitionBrowserLayout *nandBrowser;
            InstallLayout *nspInstall;
            USBInstallLayout *usbInstall;
            ThemeInstallLayout *themeInstall;
            TitleManagerLayout *titleManager;
            TicketManagerLayout *ticketManager;
            CFWConfigLayout *cfwConfig;
            SystemInfoLayout *sysInfo;
            AboutLayout *about;
            pu::element::Image *bannerImage;
            pu::element::TextBlock *timeText;
            pu::element::Image *batteryImage;
            pu::element::Image *batteryChargeImage;
            pu::element::TextBlock *footerText;
    };

    void UpdateClipboard(std::string Path);
    void ShowRebootShutDownDialog(std::string Title, std::string Message);
    void SetMainApplication(MainApplication *MainApp);
}