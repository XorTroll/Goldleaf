#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdexcept>
#include <memory>
#include <switch.h>

#include "nx/ipc/tin_ipc.h"

#include "ui/ui_mode.hpp"
#include "ui/ui_installextracted_mode.hpp"
#include "ui/ui_installnsp_mode.hpp"
#include "ui/ui_deletecommonticket_mode.hpp"
#include "ui/ui_deletepersonalizedticket_mode.hpp"
#include "ui/view.hpp"
#include "ui/console_options_view.hpp"

#include "debug.h"
#include "error.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include "tinfs.h"

void userAppInit(void);
void userAppExit(void);

#ifdef __cplusplus
}
#endif

// TODO: Throughout the application, error messages should reference the function name they occurred in
// TODO: Make custom error codes in some sort of logical fashion
// TOOD: Abstract the data source for NCA/xml/tik/cert data, so it can originate from (call it install_source.h)
// TODO: Prevent going back in certain views (title installation). Also hide the cursor
// TODO: Create a proper logging setup, as well as a log viewing screen
// TODO: Validate NCAs
// TODO: Verify dumps, ncaids match sha256s, installation succeess, perform proper uninstallation on failure and prior to install

bool g_shouldExit = false;

void userAppInit(void)
{
    if (R_FAILED(ncmextInitialize()))
        fatalSimple(0xBEEF);

    if (R_FAILED(ncmInitialize()))
        fatalSimple(0xBEE2);

    if (R_FAILED(nsInitialize()))
        fatalSimple(0xBEE3);

    if (R_FAILED(nsextInitialize()))
        fatalSimple(0xBEE4);

    if (R_FAILED(esInitialize()))
        fatalSimple(0xBEE5);

    // This may fail, but this doesn't matter for end users
    socketInitializeDefault();
    nxLinkInitialize();
}

void userAppExit(void)
{
    nxLinkExit();
    socketExit();
    ncmextExit();
    ncmExit();
    nsExit();
    nsextExit();
    esExit();
}

void markForExit(void)
{
    g_shouldExit = true;
}

int main(int argc, char **argv)
{
    try
    {
        Result rc = 0;
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();

        gfxInitDefault();
        manager.m_printConsole = consoleInit(NULL);
        
        // Create the tinfoil directory and subdirs on the sd card if they don't already exist. 
        // These are used throughout the app without existance checks.
        if (R_FAILED(rc = createTinfoilDirs()))
        {
            printf("main: Failed to create tinfoil dirs. Error code: 0x%08x\n", rc);
            return 0;
        }
        

        tin::ui::Category titleManCat("Title Management");
        titleManCat.AddMode(std::move(std::make_unique<tin::ui::InstallNSPMode>()));
        titleManCat.AddMode(std::move(std::make_unique<tin::ui::InstallExtractedNSPMode>()));
        // TODO: Add uninstall and dump nsp

        tin::ui::Category tikManCat("Ticket Management");
        tikManCat.AddMode(std::move(std::make_unique<tin::ui::DeleteCommonTicketMode>()));
        tikManCat.AddMode(std::move(std::make_unique<tin::ui::DeletePersonalizedTicketMode>()));

        // TODO: Add install tik and cert, delete personalized ticket and view title keys

        auto mainView = std::make_unique<tin::ui::ConsoleOptionsView>();

        mainView->AddEntry("Tinfoil v0.0.1 by Adubbz", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        mainView->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);
        mainView->AddEntry(titleManCat.m_name, tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&tin::ui::Category::OnSelected, &titleManCat));
        mainView->AddEntry("Install Information", tin::ui::ConsoleEntrySelectType::SELECT_INACTIVE, nullptr);
        mainView->AddEntry("Ticket Management", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&tin::ui::Category::OnSelected, &tikManCat));
        mainView->AddEntry("Exit", tin::ui::ConsoleEntrySelectType::SELECT, markForExit);
        
        manager.PushView(std::move(mainView));

        while (appletMainLoop() && !g_shouldExit)
        {
            hidScanInput();
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

            if (kDown)
                manager.ProcessInput(kDown);

            manager.Update();

            gfxFlushBuffers();
            gfxSwapBuffers();
            gfxWaitForVsync();
        }
    }
    catch (std::exception& e)
    {
        consoleClear();
        printf("An error occurred:\n%s\n\nPress any button to exit.", e.what());
        LOG_DEBUG("An error occurred:\n%s", e.what());

        u64 kDown = 0;

        while (!kDown)
        {
            hidScanInput();
            kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        }
    }

    gfxExit();
    return 0;
}