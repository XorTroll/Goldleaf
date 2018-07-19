#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <switch.h>
#include <switch/services/ncm.h>

#include "install/install.h"
#include "ipc/es.h"
#include "ipc/ncm_ext.h"
#include "ipc/ns_ext.h"
#include "ui/ui.h"
#include "ui/ui_install.h"
#include "ui/ui_ticket.h"

#include "debug.h"
#include "tinfs.h"


// TODO: Throughout the application, error messages should reference the function name they occurred in
// TODO: Make custom error codes in some sort of logical fashion
// TOOD: Abstract the data source for NCA/xml/tik/cert data, so it can originate from (call it install_source.h)
// TODO: Prevent going back in certain views (title installation). Also hide the cursor
// TODO: Create read file to buf function
// TODO: Create a proper logging setup, as well as a log viewing screen
// TODO: Improve XML validation. Check for XML presence and contents.
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
    Result rc = 0;

    gfxInitDefault();
    g_console = consoleInit(NULL);
    
    // Create the tinfoil directory and subdirs on the sd card if they don't already exist. 
    // These are used throughout the app without existance checks.
    if (R_FAILED(rc = createTinfoilDirs()))
    {
        printf("main: Failed to create tinfoil dirs. Error code: 0x%08x\n", rc);
        rc = 0;
        return rc;
    }

    View mainView = (View)
    {
        .viewEntries = 
        {
            (ViewEntry) 
            {
                .type = ViewEntryType_Heading,
                .text = "Tinfoil v0.0.1 by Adubbz",
            },
            (ViewEntry)
            {
                .type = ViewEntryType_None,
            },
            (ViewEntry)
            {
                .type = ViewEntryType_Select,
                .text = "Title Management",
                .onSelected = showTitleManagementOptionsView,
            },
            (ViewEntry)
            {
                .type = ViewEntryType_SelectInactive,
                .text = "Install Information",
            },
            (ViewEntry)
            {
                .type = ViewEntryType_Select,
                .text = "Ticket Management",
                .onSelected = showTicketManagementOptionsView
            },
            (ViewEntry)
            {
                .type = ViewEntryType_Select,
                .text = "Exit",
                .onSelected = markForExit,
            }
        },
        .numEntries = 6,
    };
    pushView(&mainView);

    while (appletMainLoop() && !g_shouldExit)
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown)
            onButtonPress(kDown);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return rc;
}