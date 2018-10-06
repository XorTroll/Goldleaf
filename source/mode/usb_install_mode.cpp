#include "mode/usb_install_mode.hpp"

extern "C"
{
#include <switch/services/hid.h>
#include <switch/display/gfx.h>
#include <switch/arm/counter.h>
#include <switch/kernel/svc.h>
}

#include <switch/kernel/svc.h>

#include <exception>
#include <sstream>
#include <stdlib.h>
#include <malloc.h>
#include <threads.h>
#include <unistd.h>
#include "ui/framework/console_view.hpp"
#include "ui/framework/console_checkbox_view.hpp"
#include "util/usb_util.hpp"
#include "nx/ipc/usb_comms_new.h"
#include "nx/ipc/usb_new.h"
#include "debug.h"
#include "error.hpp"

namespace tin::ui
{
    USBInstallMode::USBInstallMode() :
        IMode("USB Install NSP")
    {

    }

    USBInstallMode::~USBInstallMode()
    {
    }

    void USBInstallMode::OnUnwound()
    {

    }

    struct TUIHeader
    {
        u32 magic; // TUI0 (Tinfoil Usb Install 0)
        u32 nspListSize;
        u64 padding;
    } PACKED;

    void USBInstallMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleView>();
        view->m_onUnwound = std::bind(&USBInstallMode::OnUnwound, this);
        manager.PushView(std::move(view));

        Result rc = 0;
        printf("Waiting for USB to be ready...\n");

        gfxFlushBuffers();
        gfxSwapBuffers();

        while (true)
        {
            hidScanInput();
            
            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
                break;

            rc = usbDsWaitReady(1000000);

            if (R_SUCCEEDED(rc)) break;
            else if ((rc & 0x3FFFFF) != 0xEA01)
            {
                // Timeouts are okay, we just want to allow users to escape at this point
                THROW_FORMAT("Failed to wait for USB to be ready\n"); 
            }   
        }

        printf("USB is ready. Waiting for header...\n");
        
        gfxFlushBuffers();
        gfxSwapBuffers();

        TUIHeader header;
        tin::util::USBRead(&header, sizeof(TUIHeader));

        if (header.magic != 0x30495554)
            THROW_FORMAT("Incorrect TUI header magic!\n");

        LOG_DEBUG("Valid header magic.\n");
        LOG_DEBUG("NSP List Size: %u\n", header.nspListSize);

        auto nspListBuf = std::make_unique<char[]>(header.nspListSize+1);
        std::vector<std::string> nspNames;
        memset(nspListBuf.get(), 0, header.nspListSize+1);

        tin::util::USBRead(nspListBuf.get(), header.nspListSize);

        // Split the string up into individual nsp names
        std::stringstream nspNameStream(nspListBuf.get());
        std::string segment;
        std::string nspExt = ".nsp";

        while (std::getline(nspNameStream, segment, '\n'))
        {
            if (segment.compare(segment.size() - nspExt.size(), nspExt.size(), nspExt) == 0)
                nspNames.push_back(segment);
        }

        auto selectView = std::make_unique<tin::ui::ConsoleCheckboxView>(std::bind(&USBInstallMode::OnNSPSelected, this), DEFAULT_TITLE, 2);
        selectView->AddEntry("Select NSP to install", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        selectView->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);
        
        for (auto& nspName : nspNames)
        {
            LOG_DEBUG("NSP Name: %s\n", nspName.c_str());
            selectView->AddEntry(nspName, tin::ui::ConsoleEntrySelectType::SELECT, nullptr);
        }
        manager.PushView(std::move(selectView));
    }

    void USBInstallMode::OnNSPSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleCheckboxView* prevView;

        if (!(prevView = dynamic_cast<ConsoleCheckboxView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("Previous view must be a ConsoleCheckboxView!");
        }

        auto values = prevView->GetSelectedOptionValues();

        /*for (auto& destStr : values)
        {
            m_urls.push_back(destStr->GetText());
        }*/

        auto view = std::make_unique<tin::ui::ConsoleOptionsView>(DEFAULT_TITLE);
        view->AddEntry("Select Destination", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);
        //view->AddEntry("SD Card", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&NetworkInstallMode::OnDestinationSelected, this));
        //view->AddEntry("NAND", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&NetworkInstallMode::OnDestinationSelected, this));
        manager.PushView(std::move(view));
    }
}