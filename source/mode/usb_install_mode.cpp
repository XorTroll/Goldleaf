#include "mode/usb_install_mode.hpp"

extern "C"
{
#include <switch/services/hid.h>
#include <switch/display/gfx.h>
#include <switch/arm/counter.h>
#include <switch/kernel/svc.h>
}

#include <switch/kernel/svc.h>

#include <threads.h>
#include <unistd.h>
#include "ui/framework/console_view.hpp"
#include "nx/ipc/usb_comms_new.h"
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
        usbCommsExit();
    }

    bool g_exit = false;
    size_t g_sizeBuffered = 0;

    int USBSpeedFunc(void* in)
    {
        u8* tmp = (u8*)malloc(0x800000);

        while (!g_exit)
        {
            g_sizeBuffered += usbCommsRead(tmp, 512);
        }
        
        free(tmp);
        return 0;
    }

    void USBInstallMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleView>();
        view->m_onUnwound = std::bind(&USBInstallMode::OnUnwound, this);
        manager.PushView(std::move(view));
    
        ASSERT_OK(usbCommsInitialize(), "Failed to initialize usb comms!\n");
        printf("USB comms initialized\nAwaiting input...\n");

        thrd_t usbThread;

        thrd_create(&usbThread, USBSpeedFunc, NULL);
        u64 freq = armGetSystemTickFreq();
        u64 startTime = armGetSystemTick();
        
        double speed = 0.0;

        while (true)
        {
            hidScanInput();
            
            if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
            {
                g_exit = true;
                break;
            }

            u64 newTime = armGetSystemTick();

            if (newTime - startTime >= freq)
            {
                double mbBuffered = (g_sizeBuffered / 1000000.0);
                double duration = ((double)(newTime - startTime) / (double)freq);
                speed =  mbBuffered / duration;

                startTime = newTime;
                g_sizeBuffered = 0;

                printf("%.2f MB/s\r", speed);
            }

            gfxFlushBuffers();
            gfxSwapBuffers();
            //svcSleepThread(5000000);
        }
        
        thrd_join(usbThread, NULL);

        //if (magic != 0x30495554)
            //LOG_DEBUG("Incorrect magic!\n");

        //printBytes(nxlinkout, (u8*)&magic, sizeof(u32), true);
        //LOG_DEBUG("Magic: 0x:%ux\n", magic);
    }
}