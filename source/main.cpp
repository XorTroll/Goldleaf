#include <iostream>
#include <switch.h>
#include "ncm.h"
#include "console_select_panel.hpp"
#include "menu.hpp"
#include "panel_changer.hpp"

extern "C"
{
    void userAppInit(void);
    void userAppExit(void);
}

void userAppInit(void)
{
    Result rc;

    if (R_FAILED(rc = ncmInitialize()))
        fatalSimple(0xCAFE << 4 | 4);

    if (R_FAILED(rc = nsInitialize()))
        fatalSimple(0xCAFE << 4 | 5);
}

void userAppExit(void)
{
    ncmExit();
    nsExit();
}

int main(int argc, char **argv)
{
    Result rc;
    gfxInitDefault();
    PrintConsole *console = consoleInit(NULL);

    menu::Menu menu(console);
    menu::g_menu = &menu;

    auto mainPanel = std::make_shared<menu::ConsoleSelectPanel>(console, "Tinfoil 0.0.1");
    auto mainSection = mainPanel->addSection("Main Menu");
    mainSection->addEntry("Title Info", std::bind(menu::main_menu::titleInfoSelected));
    mainSection->addEntry("Exit", std::bind(menu::main_menu::exitSelected));

    menu.pushPanel(mainPanel);

    while (appletMainLoop() && !menu.m_exitRequested)
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    
        if (kDown & KEY_PLUS)
            break;

        if (kDown != 0)
            menu.processInput(kDown);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}