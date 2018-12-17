
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
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
            void titleMenuItem_Click();
            void ticketMenuItem_Click();
            void sysinfoMenuItem_Click();
        private:
            pu::element::Menu *optionMenu;
            pu::element::MenuItem *sdcardMenuItem;
            pu::element::MenuItem *nandMenuItem;
            pu::element::MenuItem *remoteMenuItem;
            pu::element::MenuItem *titleMenuItem;
            pu::element::MenuItem *ticketMenuItem;
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
            void fsItems_Click();
            void AlertInstallerError(gleaf::nsp::InstallerResult Res);
        private:
            gleaf::fs::Explorer *gexp;
            pu::element::Menu *browseMenu;
            pu::element::TextBlock *dirEmptyText;
    };

    class NSPInstallLayout : public pu::Layout
    {
        public:
            NSPInstallLayout();
            void StartInstall(gleaf::nsp::Installer *Inst, pu::Layout *Prev);
            void LogError(gleaf::nsp::InstallerResult Res);
        private:
            pu::element::TextBlock *installText;
            pu::element::ProgressBar *installBar;
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

    class SystemInfoLayout : public pu::Layout
    {
        public:
            SystemInfoLayout();
            void UpdateElements();
        private:
            pu::element::TextBlock *fwText;
            pu::element::TextBlock *sdText;
            pu::element::ProgressBar *sdBar;
            pu::element::TextBlock *nandText;
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
            pu::element::TextBlock *typeText;
    };

    class MainApplication : public pu::Application
    {
        public:
            MainApplication();
            void UpdateFooter(std::string Text);
            void UpdateValues();
            void sdBrowser_Input(u64 Input);
            void nandBrowser_Input(u64 Input);
            void titleManager_Input(u64 Input);
            void ticketManager_Input(u64 Input);
            void sysInfo_Input(u64 Input);
            PartitionBrowserLayout *GetSDBrowserLayout();
            PartitionBrowserLayout *GetNANDBrowserLayout();
            NSPInstallLayout *GetNSPInstallLayout();
            TitleManagerLayout *GetTitleManagerLayout();
            TicketManagerLayout *GetTicketManagerLayout();
            SystemInfoLayout *GetSystemInfoLayout();
        private:
            MainMenuLayout *mainMenu;
            PartitionBrowserLayout *sdBrowser;
            PartitionBrowserLayout *nandBrowser;
            NSPInstallLayout *nspInstall;
            TitleManagerLayout *titleManager;
            TicketManagerLayout *ticketManager;
            SystemInfoLayout *sysInfo;
            pu::element::Image *bannerImage;
            pu::element::TextBlock *timeText;
            pu::element::Image *batteryImage;
            pu::element::Image *batteryChargeImage;
            pu::element::TextBlock *footerText;
    };

    void SetMainApplication(MainApplication *MainApp);
}