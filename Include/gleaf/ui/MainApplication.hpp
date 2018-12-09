
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/gleaf.hpp>
#include <pn/pn.hpp>

namespace gleaf::ui
{
    class MainMenuLayout : public pn::Layout
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
            pn::fw::Menu *optionMenu;
            pn::fw::MenuItem *sdcardMenuItem;
            pn::fw::MenuItem *nandMenuItem;
            pn::fw::MenuItem *remoteMenuItem;
            pn::fw::MenuItem *titleMenuItem;
            pn::fw::MenuItem *ticketMenuItem;
            pn::fw::MenuItem *sysinfoMenuItem;
            pn::fw::MenuItem *aboutMenuItem;
    };

    class PartitionBrowserLayout : public pn::Layout
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
            pn::fw::Menu *browseMenu;
            pn::fw::TextBlock *dirEmptyText;
    };

    class NSPInstallLayout : public pn::Layout
    {
        public:
            NSPInstallLayout();
            void StartInstall(gleaf::nsp::Installer *Inst, pn::Layout *Prev);
            void LogError(gleaf::nsp::InstallerResult Res);
        private:
            pn::fw::TextBlock *installText;
            pn::fw::ProgressBar *installBar;
    };

    class TitleManagerLayout : public pn::Layout
    {
        public:
            TitleManagerLayout();
            void UpdateElements();
            void titles_Click();
            std::vector<gleaf::horizon::Title> GetTitles();
        private:
            std::vector<gleaf::horizon::Title> titles;
            pn::fw::TextBlock *notTitlesText;
            pn::fw::Menu *titlesMenu;
    };

    class TicketManagerLayout : public pn::Layout
    {
        public:
            TicketManagerLayout();
            void UpdateElements();
            void tickets_Click();
        private:
            std::vector<gleaf::horizon::Ticket> tickets;
            pn::fw::TextBlock *notTicketsText;
            pn::fw::Menu *ticketsMenu;
    };

    class SystemInfoLayout : public pn::Layout
    {
        public:
            SystemInfoLayout();
            void UpdateElements();
        private:
            pn::fw::TextBlock *fwText;
            pn::fw::TextBlock *sdText;
            pn::fw::ProgressBar *sdBar;
            pn::fw::TextBlock *nandText;
            pn::fw::TextBlock *safeText;
            pn::fw::ProgressBar *safeBar;
            pn::fw::TextBlock *systemText;
            pn::fw::ProgressBar *systemBar;
            pn::fw::TextBlock *userText;
            pn::fw::ProgressBar *userBar;
    };

    class AboutLayout : public pn::Layout
    {
        public:
            AboutLayout();
        private:
            pn::fw::TextBlock *typeText;
    };

    class MainApplication : public pn::Application
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
            pn::fw::Image *bannerImage;
            pn::fw::TextBlock *timeText;
            pn::fw::Image *batteryImage;
            pn::fw::Image *batteryChargeImage;
            pn::fw::TextBlock *footerText;
    };

    void SetMainApplication(MainApplication *MainApp);
}